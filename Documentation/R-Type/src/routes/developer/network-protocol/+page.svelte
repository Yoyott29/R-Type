<script>
	const layersExample = `UDP datagram (at most 1200 bytes)
└── NetworkPlugin packet          header: protocolId u16, kind u8, seq u16      (5 bytes)
    └── Data packet: messages     each: type u16, len u16, payload (len bytes)  (4 + len bytes)
        └── R-Type payload        Welcome, Input or Snapshot, described below`;

	const messagesExample = `message    type   payload                                          size        direction
Welcome    1      playerId (u8)                                    1           server -> client
Input      2      tick (u32), buttons (u8)                         5           client -> server
Snapshot   3      tick (u32), count (u16), entities...             6 + 11 * n  server -> client`;

	const inputLayout = `offset  size  field     notes
0       4     tick      the client's own counter, starts at 0, +1 per Input (not read by the server)
4       1     buttons   InputBits, see below; bits 0xE0 MUST be 0`;

	const snapshotLayout = `offset  size     field      notes
0       4        tick       server tick of this snapshot
4       2        count      number of entities in THIS message, not in the whole snapshot
6       11 * n   entities   n = count entity records, see below`;

	const entityLayout = `offset  size  field     type   notes
0       4     x         f32    centre of the entity, in pixels
4       4     y         f32    centre of the entity, in pixels
8       1     type      u8     EntType: PLAYER 0, ENEMY 1, BULLET 2, OBSTACLE 3, OTHER 4
9       1     playerId  u8     0 to 3 for a PLAYER, 0xFF (NO_PLAYER) otherwise
10      1     variant   u8     0 = default, 1 = ENEMY_VARIANT_2 (second kind of enemy)`;

	const valuesExample = `buttons   UP = 0x01, DOWN = 0x02, LEFT = 0x04, RIGHT = 0x08, CHARGE = 0x10
          bits 0xE0 (INPUT_UNUSED_BITS) must be 0
type      PLAYER = 0, ENEMY = 1, BULLET = 2, OBSTACLE = 3, OTHER = 4 (stars)
variant   DEFAULT_VARIANT = 0, ENEMY_VARIANT_2 = 1
player    NO_PLAYER = 0xFF, MAX_PLAYERS = 4`;

	const stateExample = `            NetworkPlugin handshake (ConnectRequest every 200 ms, 25 tries)
 START ───────────────────────────> CONNECTING
                                      │  ConnectRejected / no answer ──────> STOPPED
                                      │  ConnectAccepted
                                      v
                                   CONNECTED
   every frame: send Input            │  first Welcome: store the playerId
   every Snapshot: redraw             │  Disconnect from the server ───────> STOPPED
                                      │  nothing received for 5 s ─────────> STOPPED`;

	const bytesExample = `Welcome, you are player 0 (one Data packet, seq 7)
54 52  00  07 00  |  01 00  01 00  |  00
 id   kind  seq      type    len      playerId

Input of tick 1, buttons UP + RIGHT
02 00  05 00  |  01 00 00 00  09
type    len      tick         buttons

Snapshot of tick 5 with one entity: player 0, centred at (100, 200)
03 00  11 00  |  05 00 00 00  01 00  |  00 00 c8 42  00 00 48 43  00  00  00
type   len=17    tick         count     x = 100      y = 200      ty  id  va`;

	const codeExample = `// src/games/rtype/common/RTypeMessages.hpp
std::vector<std::byte> bytes = encodeInput({tick, buttons});
transport.send(0, static_cast<std::uint16_t>(RTypeMessage::Input), bytes);

// on the other side, NetError is thrown when the payload is malformed
InputMessage input = decodeInput(message.payload);`;

	const checklistExample = `1. implement the NetworkPlugin transport (src/plugins/Network/Packet.hpp): handshake,
   seq numbers, KeepAlive, Disconnect; protocolVersion = 1
2. connect to the server (default 127.0.0.1:4242)
3. every frame (about 60 per second), send a 5 byte Input with the buttons held
4. on Welcome, store playerId (0 to 3); later Welcomes are repeats, ignore them
5. for every Snapshot message:
     older tick than the one on screen   -> drop it
     newer tick                          -> clear the screen, draw its entities
     same tick                           -> it is another part, add its entities
6. pick the look of each entity from its type, variant and playerId
7. stop when the server sends Disconnect or stays silent for 5 s`;
</script>

<svelte:head>
	<title>How the protocol works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How the protocol works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	The specification of the R-Type messages sent between the clients and the server
</p>

<p class="max-w-[100ch] leading-7">
	This page is the reference to write a new client for the server. The words
	<strong>MUST</strong>, <strong>SHOULD</strong> and <strong>MAY</strong> are used as in
	<a href="https://www.rfc-editor.org/rfc/rfc2119" class="text-[#ff8a3d] hover:underline">RFC 2119</a>.
	The sources of truth are {@render c('src/games/rtype/common/RTypeMessages.hpp')} (the messages and
	their encoding) and {@render c('src/games/rtype/common/rtype_protocol.hpp')} (the constants),
	shared by the client and the server.
</p>

<h2 class="mt-9 text-lg font-semibold">1. Transport</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The game does not touch the socket. Every message goes through the
	{@render c('NetworkPlugin')}, which handles the connection, the sequence numbers, the keep-alives
	and the timeouts. Its wire format is defined in {@render c('src/plugins/Network/Packet.hpp')}. This
	page only describes the payloads the game puts inside it.
</p>
{@render code(layersExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Every message is sent over <strong>UDP</strong>, once. It may be lost or arrive out of order.</li>
	<li>
		All numbers are <strong>little-endian</strong>. Floats ({@render c('f32')}) are IEEE 754 single
		precision, copied bit by bit ({@render c('ByteWriter')} / {@render c('ByteReader')}).
	</li>
	<li>
		A message payload is at most 1191 bytes ({@render c('MAX_PAYLOAD')}). Several messages for the
		same peer are packed into one datagram when they fit.
	</li>
	<li>
		The server listens on {@render c('127.0.0.1:4242')} by default. It must be started with
		{@render c('./r-type_server 0.0.0.0 4242')} to accept clients from other machines.
	</li>
	<li>
		The handshake checks {@render c('NetworkConfig::protocolVersion')} (1). A client with another
		version is rejected with {@render c('VersionMismatch')}.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">2. The messages</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The message type is the {@render c('u16')} of the NetworkPlugin message header
	({@render c('RTypeMessage')}). A receiver MUST ignore any type it does not know.
	{@render c('n')} is the number of entities in the message.
</p>
{@render code(messagesExample)}

<h3 class="mt-6 font-semibold">Welcome (1), server to client</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	One byte, the player id, from {@render c('0')} to {@render c('3')}. The server sends it when the
	client connects, then again with every {@render c('Input')} it receives, since the first one can
	be lost. A client MUST keep the first one and ignore the repeats. If the four slots are taken, the
	server disconnects the new peer instead.
</p>

<h3 class="mt-6 font-semibold">Input (2), client to server</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	Exactly 5 bytes, or the server ignores it. A connected client SHOULD send one per frame, even when
	no button is held.
</p>
{@render code(inputLayout)}
<p class="mt-3 max-w-[100ch] leading-7">
	The server only reads the {@render c('buttons')} byte. While its fire cooldown (0.2 s) is over, the
	server fires a bullet for every input it applies, so {@render c('CHARGE')} has no effect today.
</p>

<h3 class="mt-6 font-semibold">Snapshot (3), server to client</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	The full state of the game, sent to every connected player at each new server tick.
</p>
{@render code(snapshotLayout)}
<p class="mt-3 max-w-[100ch] leading-7">
	Each entity record is 11 bytes ({@render c('SNAPSHOT_ENTITY_BYTES')}). It says what the entity
	<em>is</em>, never how it looks:
</p>
{@render code(entityLayout)}

<h2 class="mt-9 text-lg font-semibold">3. Values</h2>
{@render code(valuesExample)}

<h2 class="mt-9 text-lg font-semibold">4. Snapshot parts and coordinates</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		A message holds at most (1191 - 6) / 11 = <strong>107</strong> entities
		({@render c('SNAPSHOT_ENTITIES_PER_CHUNK')}). A bigger snapshot is sent as several
		{@render c('Snapshot')} messages with the same tick. There is no part index and no total count.
	</li>
	<li>
		A client SHOULD drop a snapshot older than the one on screen, replace everything when a newer
		tick arrives, and add the entities of a late part of the same tick. See
		<a href="/developer/network-snapshots" class="text-[#ff8a3d] hover:underline">How snapshots work</a>.
	</li>
	<li>
		The world is 1920 x 1080 pixels, origin at the top left, y going down. A position is the centre
		of the entity. Stars ({@render c('OTHER')}) and enemies can be slightly outside this area.
	</li>
	<li>
		The client decides how each entity looks, from its type, its variant and, for a player, its
		player id (see {@render c('spriteForType')} in {@render c('ClientNetSystem.cpp')} and
		<a href="/user/assets" class="text-[#ff8a3d] hover:underline">Assets and Sprites</a>).
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">5. Session and timing</h2>
{@render code(stateExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The server runs 60 ticks per second. It sends one snapshot per new tick, so at most one per
		frame.
	</li>
	<li>
		A peer that sends nothing for <strong>5 seconds</strong> is dropped. The NetworkPlugin sends a
		{@render c('KeepAlive')} after 250 ms of silence, so an idle link stays up.
	</li>
	<li>
		The server keeps the 2 newest inputs of each player and applies one per tick. A client SHOULD
		NOT send more than 60 inputs per second, or its oldest inputs are dropped.
	</li>
	<li>
		A player hit by an enemy is eliminated: the server disconnects it, and the client stops. To
		play again, start a new client.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">6. Malformed messages</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The decoders ({@render c('decodeWelcome')}, {@render c('decodeInput')},
	{@render c('decodeSnapshot')}) throw {@render c('NetError')} on a bad payload, and the systems
	catch it and drop the message:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>A payload too short or with trailing bytes: dropped.</li>
	<li>An {@render c('Input')} with a reserved bit set: dropped.</li>
	<li>A {@render c('Welcome')} with a player id above 3: dropped.</li>
	<li>
		A {@render c('Snapshot')} whose size does not match its count, with an entity type above 4, or
		with a {@render c('PLAYER')} id above 3: the whole message is dropped.
	</li>
	<li>An unknown message type, or an {@render c('Input')} from a peer with no ship: ignored.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">7. Examples</h2>
<p class="mt-3 max-w-[100ch] leading-7">The encode / decode helpers are shared by both sides:</p>
{@render code(codeExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Some messages, byte by byte. The first one shows the NetworkPlugin packet around it, the others
	only the message:
</p>
{@render code(bytesExample)}

<h2 class="mt-9 text-lg font-semibold">8. Writing a new client</h2>
{@render code(checklistExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Change {@render c('NetworkConfig::protocolVersion')} whenever the format of a message changes,
		so that an old client is rejected with {@render c('VersionMismatch')} instead of having its
		messages dropped. Update this page at the same time.
	</li>
	<li>
		There is no authentication: the server trusts the source address. See the security part of the
		<a href="/developer/technical-study" class="text-[#ff8a3d] hover:underline">technical study</a>.
	</li>
	<li>
		{@render c('tests/netlag.sh')} and {@render c('tests/netloss.sh')} add delay or packet loss on the loopback
		interface, to see how the game behaves on a bad network.
	</li>
</ul>
