<script>
	const flowExample = `client ──UDP──> network thread ──push──> one InputQueue per player ──pop──> game loop (60 ticks / s)
client <──UDP── SNAPSHOT of the tick <──────────────────────────────────────── game loop`;

	const sceneExample = `each tick
  new player       -> createEntity + Position, Velocity, Sprite   (_players[id] = entity)
  input of a tick  -> sets the Velocity of the player's entity
  FixedUpdate      -> MovementSystem: Position += Velocity * FIXED_DELTA_TIME
  buildSnapshot()  -> one entry per player: its Position and Type PLAYER`;

	const connectExample =`client                                server
  [CONNECT, version] ──────────────────>  known version and a free slot?
                     <──────────────────  [CONNECT_ACCEPT, playerId]   (0 to 3)
                     <──────────────────  [CONNECT_REJECT, reason]     (otherwise)`;

	const packetExample = `CONNECT         0x10   [0x10, version]                                   2 bytes    client -> server
INPUT           0x30   [0x30, tick (u32), keys, lastSnapshotTick (u32)]  10 bytes   client -> server
CONNECT_ACCEPT  0x90   [0x90, playerId]                                  2 bytes    server -> client
CONNECT_REJECT  0x91   [0x91, reason]                                    2 bytes    server -> client
SNAPSHOT        0xC0   [0xC0, tick (u32), count (u16), entities...]      7 + 9 per entity

entity   [x (f32), y (f32), type (u8)]
keys     UP = 0x01, DOWN = 0x02, LEFT = 0x04, RIGHT = 0x08, CHARGE = 0x10
reason   SERVER_FULL = 0x01, WRONG_VERSION = 0x02, NOT_CONNECTED = 0x04`;

	const runExample = `./r-type_server          # port 4242
./r-type_server 5000     # any port from 1 to 65535
./r-type_client          # connects to 127.0.0.1:4242`;

	const sendExample = `# CONNECT with protocol version 1, prints the reply
printf '\\x10\\x01' | nc -u -w1 127.0.0.1 4242 | od -An -tx1
 90 00    → CONNECT_ACCEPT, you are player 0

# an INPUT from an address that never sent CONNECT
printf '\\x30\\x00\\x00\\x00\\x01\\x05\\x00\\x00\\x00\\x00' | nc -u -w1 127.0.0.1 4242 | od -An -tx1
 91 04    → CONNECT_REJECT, NOT_CONNECTED`;

	const outputExample = `Server listening on port : 4242
Player 0 connected: 127.0.0.1:51391`;
</script>

<svelte:head>
	<title>How the server works - R-Type docs</title>
</svelte:head>

{#snippet code(text)}
	<pre
		class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code
			>{text}</code
		></pre>
{/snippet}

{#snippet c(text)}<code
		class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]"
		>{text}</code
	>{/snippet}

<h1 class="text-3xl font-semibold">How the server works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How clients connect to the game server, how their inputs reach the game loop and how the state is sent back
</p>

<p class="max-w-[100ch] leading-7">
	{@render c('r-type_server')} talks to the clients over UDP with Asio, on port
	{@render c('4242')} by default. Up to 4 players can connect. This part will still change a lot,
	so this page only covers what works today.
</p>

<h2 class="mt-9 text-lg font-semibold">How it works</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A network thread receives every packet on a single socket and finds out which player sent it from
	its address. Each player has their own input queue, so inputs never get mixed. At each tick, the
	game loop takes one input from each player's queue, runs the game scene for one tick, then sends a
	snapshot of the game state to every connected player.
</p>
{@render code(flowExample)}

<h2 class="mt-9 text-lg font-semibold">The game scene</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('GameLoop')} owns its own ECS {@render c('Scene')}, separate from the one of each
	client. It registers {@render c('Position')}, {@render c('Velocity')}, {@render c('Type')} and
	{@render c('Sprite')}, the {@render c('TickTimeManager')} resource, and the
	{@render c('MovementSystem')} in {@render c('FixedUpdate')}. When a player connects, the game loop
	creates an entity for them and keeps it in {@render c('_players')}, which maps each player id to
	its entity. An input sets the velocity of the player's entity, then
	{@render c('runSchedule(FixedUpdate)')} moves it.
</p>
{@render code(sceneExample)}

<h2 class="mt-9 text-lg font-semibold">Connecting</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A client must send {@render c('CONNECT')} before anything else. The server checks the protocol
	version, gives the client the smallest free player id and replies. Any other message from an
	address that is not connected is refused with {@render c('NOT_CONNECTED')}. If the client sends
	{@render c('CONNECT')} again because the reply got lost, it gets the same id back. The client
	sends {@render c('CONNECT')} again every few moments until it is accepted, then starts sending
	its inputs every frame.
</p>
{@render code(connectExample)}

<h2 class="mt-9 text-lg font-semibold">The packets</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The format is shared by the client and the server in
	{@render c('src/common/rtype_protocol.hpp')}. The first byte is the message type: bit 7 is the
	direction (0 from the client, 1 from the server). Numbers bigger than one byte are sent in
	big-endian with {@render c('writeValue')} and {@render c('readValue')}. These messages are
	handled today:
</p>
{@render code(packetExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The file also defines the next messages ({@render c('DISCONNECT')}, {@render c('GAME_START')},
	{@render c('DELTA')}...), which the server does not handle yet.
</p>

<h2 class="mt-9 text-lg font-semibold">Snapshots</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('GameLoop::buildSnapshot')} reads the {@render c('Position')} of every player entity
	in the scene. {@render c('Network::sendSnapshot')} keeps the last 64 snapshots in a history and
	sends the entities of the tick to every player. If they do not fit in one 1024 bytes packet, they are split
	into several packets with the same tick. The client rebuilds its scene from each snapshot it
	receives and sends the last tick it got back in its next {@render c('INPUT')}.
</p>

<h2 class="mt-9 text-lg font-semibold">Testing it</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Start the server from the repository root, with an optional port:
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Or, in another terminal, send packets with {@render c('nc')} and read the reply with
	{@render c('od')}. Each {@render c('nc')} uses a new port, so the server sees a new client every
	time:
</p>
{@render code(sendExample)}
<p class="mt-3 max-w-[100ch] leading-7">The server logs each connection:</p>
{@render code(outputExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Only the players are sent in the snapshot for now, all with the {@render c('PLAYER')} type.
		The client picks the sprite to draw from that type, so the {@render c('Sprite')} of the server
		is never sent.
	</li>
	<li>
		The server and the clients have separate scenes, so their entity ids do not match. A network
		id will be needed to sync them.
	</li>
	<li>
		The server only reads the keys of an {@render c('INPUT')}: the tick and the snapshot
		acknowledgement are not used yet.
	</li>
	<li>Players are never removed yet: there is no disconnect or timeout.</li>
	<li>
		Press Ctrl+C (SIGINT) or send SIGTERM to stop the server cleanly. It does not read stdin, so
		it can run under nohup, docker or CI.
	</li>
</ul>
