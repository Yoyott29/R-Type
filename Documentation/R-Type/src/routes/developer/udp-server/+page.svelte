<script>
	const flowExample = `client ──UDP──> network thread ──push──> one InputQueue per player ──pop──> game loop (60 ticks / s)
client <──UDP── SNAPSHOT of the tick <──────────────────────────────────────── game loop`;

	const sceneExample = `each tick
  timeouts           -> players silent for 2 s are dropped, their entity destroyed
  new player         -> createEntity + Type, PlayerId, Position, Velocity, Sprite, Collider2D, Script
  input of a tick    -> sets the Velocity of the player, shoots a PlayerBullet if the cooldown is over
  FixedUpdate        -> MovementSystem, ScriptSystem (enemy and star spawners, off-screen), Collider2DSystem
  flushDestroyQueue  -> entities destroyed during the tick are removed
  buildSnapshot()    -> position, type and look of every player, enemy, bullet and star`;

	const connectExample =`client                                server
  [CONNECT, version] ──────────────────>  known version and a free slot?
                     <──────────────────  [CONNECT_ACCEPT, playerId]   (0 to 3)
                     <──────────────────  [CONNECT_REJECT, reason]     (otherwise)`;

	const runExample = `./r-type_server                    # 127.0.0.1:4242
./r-type_server 5000               # 127.0.0.1:5000
./r-type_server 192.168.0.39 4444  # a given ip and port, to play from other machines
./r-type_client                    # connects to 127.0.0.1:4242`;

	const sendExample = `# CONNECT with protocol version 3, prints the reply
printf '\\x10\\x03' | nc -u -w1 127.0.0.1 4242 | od -An -tx1
 90 00    → CONNECT_ACCEPT, you are player 0

# an INPUT from an address that never sent CONNECT
printf '\\x30\\x00\\x00\\x00\\x01\\x05\\x00\\x00\\x00\\x00' | nc -u -w1 127.0.0.1 4242 | od -An -tx1
 91 04    → CONNECT_REJECT, NOT_CONNECTED`;

	const outputExample = `Server listening on ip : 127.0.0.1
Server listening on port : 4242
Player 0 connected: 127.0.0.1:51391
Player 0 timed out
Closing server`;
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
	{@render c('r-type_server')} talks to the clients over UDP with Asio, on
	{@render c('127.0.0.1:4242')} by default. Up to 4 players can connect. This page gives the big
	picture, the next pages of the Network section detail each part.
</p>

<h2 class="mt-9 text-lg font-semibold">How it works</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A network thread receives every packet on a single socket and finds out which player sent it from
	its address. Each player has their own input queue, so inputs never get mixed. At each tick, the
	game loop takes one input from each player's queue, runs the game scene for one tick, then sends a
	snapshot of the game state to every connected player. The two threads are described in
	<a href="/developer/network-threads" class="text-[#ff8a3d] hover:underline">How the server threads work</a>.
</p>
{@render code(flowExample)}

<h2 class="mt-9 text-lg font-semibold">The game scene</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('GameLoop')} owns its own ECS {@render c('Scene')}, separate from the one of each
	client. It registers the game components, the {@render c('TickTimeManager')} resource, and the
	{@render c('MovementSystem')}, {@render c('ScriptSystem')} and {@render c('Collider2DSystem')}
	in {@render c('FixedUpdate')}. Every player, enemy and bullet it creates is kept in
	{@render c('_entities')}, which is what the snapshots are built from. Enemies come in waves from
	a spawner script, bullets from the {@render c('PlayerBullet')} prefab.
</p>
{@render code(sceneExample)}

<h2 class="mt-9 text-lg font-semibold">Connecting</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A client must send {@render c('CONNECT')} before anything else. The server checks the protocol
	version, gives the client the smallest free player id and replies. Any other message from an
	address that is not connected is refused with {@render c('NOT_CONNECTED')}. The details are in
	<a href="/developer/network-connection" class="text-[#ff8a3d] hover:underline">How connecting works</a>.
</p>
{@render code(connectExample)}

<h2 class="mt-9 text-lg font-semibold">The packets</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The format is shared by the client and the server in
	{@render c('src/common/rtype_protocol.hpp')}: {@render c('CONNECT')},
	{@render c('DISCONNECT')} and {@render c('INPUT')} from the client,
	{@render c('CONNECT_ACCEPT')}, {@render c('CONNECT_REJECT')}, {@render c('PLAYER_LEFT')} and
	{@render c('SNAPSHOT')} from the server. Every packet is described byte by byte in
	<a href="/developer/network-protocol" class="text-[#ff8a3d] hover:underline">How the protocol works</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Snapshots</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('GameLoop::buildSnapshot')} reads the {@render c('Position')}, the
	{@render c('Type')}, the {@render c('PlayerId')} and the {@render c('Sprite')} (size, shape, color,
	draw order) of every entity in {@render c('_entities')}.
	{@render c('Network::sendSnapshot')} keeps the last 64 snapshots in a history and sends the
	entities of the tick to every player. If they do not fit in one 1024 bytes packet, they are split
	into several packets with the same tick. The client shows the newest tick it received, see
	<a href="/developer/network-snapshots" class="text-[#ff8a3d] hover:underline">How snapshots work</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Testing it</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Start the server from the repository root, with an optional ip and port:
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Or, in another terminal, send packets with {@render c('nc')} and read the reply with
	{@render c('od')}. Each {@render c('nc')} uses a new port, so the server sees a new client every
	time:
</p>
{@render code(sendExample)}
<p class="mt-3 max-w-[100ch] leading-7">The server logs what happens:</p>
{@render code(outputExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('tests/tests.sh')} runs 109 of these checks automatically, see
	<a href="/developer/tests-sh" class="text-[#ff8a3d] hover:underline">How tests.sh works</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The size, shape, color and draw order of the server {@render c('Sprite')} are sent, but not its
		texture path: the client picks the texture from the type, the variant and the player id.
	</li>
	<li>
		The server and the clients have separate scenes, so their entity ids do not match. A network
		id will be needed to sync them.
	</li>
	<li>
		Players leave with {@render c('DISCONNECT')}, after 2 seconds of silence, or when they are hit,
		see
		<a href="/developer/network-disconnect" class="text-[#ff8a3d] hover:underline">How disconnects and timeouts work</a>.
	</li>
	<li>
		Press Ctrl+C (SIGINT) or send SIGTERM to stop the server cleanly. It does not read stdin, so
		it can run under nohup, docker or CI.
	</li>
	<li>
		With no ip, the server only accepts clients from the same machine. Give the ip of the machine
		to accept players from other computers.
	</li>
</ul>
