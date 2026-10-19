<script>
	const packetsExample = `type            byte   content                                            size        direction
CONNECT         0x10   [0x10, version]                                    2           client -> server
DISCONNECT      0x11   [0x11]                                             1           client -> server
INPUT           0x30   [0x30, tick (u32), keys, lastSnapshotTick (u32)]   10          client -> server
CONNECT_ACCEPT  0x90   [0x90, playerId]                                   2           server -> client
CONNECT_REJECT  0x91   [0x91, reason]                                     2           server -> client
PLAYER_LEFT     0x92   [0x92, playerId]                                   2           server -> client
SNAPSHOT        0xC0   [0xC0, tick (u32), count (u16), entities...]       7 + 25 * n  server -> client

reserved, never sent by the current server or client
GAME_START      0x20   [0x20]                                             1           client -> server
GAME_STARTED    0xA0   [0xA0, startTick (u32)]                            5           server -> client
DELTA           0xC1   [0xC1, tick (u32), baseTick (u32), count (u16), entities...]   server -> client`;

	const firstByteExample = `bit    7          6 5 4        3 2 1 0
       direction  category     index
       0 = client -> server
       1 = server -> client (SERVER_TO_CLIENT_BIT = 0x80)

category  1 = session (CONNECT, DISCONNECT, CONNECT_ACCEPT, ...)
          2 = game    (GAME_START, GAME_STARTED)
          3 = input   (INPUT)
          4 = state   (SNAPSHOT, DELTA, seen as 0xC_ because of the direction bit)`;

	const inputLayout = `offset  size  field             notes
0       1     type              0x30
1       4     tick (u32)        the client's own frame counter, starts at 0, +1 per INPUT
5       1     keys              InputBits, see below
6       4     lastSnapshotTick  tick of the newest SNAPSHOT the client applied (0 before any)`;

	const snapshotLayout = `offset  size     field         notes
0       1        type          0xC0
1       4        tick (u32)    server tick of this snapshot (the 64 bit tick, truncated)
5       2        count (u16)   number of entities in THIS datagram, not in the whole snapshot
7       25 * n   entities      n = count entity records, see below`;

	const entityLayout = `offset  size  field       type   notes
0       4     x           f32    centre of the entity, in pixels
4       4     y           f32    centre of the entity, in pixels
8       1     type        u8     EntType: PLAYER 0, ENEMY 1, BULLET 2, OBSTACLE 3, OTHER 4
9       1     playerId    u8     0 to 3 for a PLAYER, 0xFF (NO_PLAYER) otherwise
10      1     variant     u8     0 = default, 1 = ENEMY_VARIANT_2 (second Bydos sprite)
11      4     width       f32    box width, or the radius when shape is a circle
15      4     height      f32    box height, or the radius when shape is a circle
19      1     shape       u8     0 = box, 1 = circle
20      1     drawOrder   i8     smaller is drawn first (stars are -1, behind everything)
21      4     color       u8[4]  r, g, b, a`;

	const valuesExample = `keys      UP = 0x01, DOWN = 0x02, LEFT = 0x04, RIGHT = 0x08, CHARGE = 0x10
          bits 0xE0 (INPUT_UNUSED_BITS) must be 0
reason    SERVER_FULL = 0x01, WRONG_VERSION = 0x02, GAME_ALREADY_STARTED = 0x03 (reserved),
          NOT_CONNECTED = 0x04
type      PLAYER = 0, ENEMY = 1, BULLET = 2, OBSTACLE = 3, OTHER = 4 (stars)
variant   DEFAULT_VARIANT = 0, ENEMY_VARIANT_2 = 1
shape     BOX_SHAPE = 0, CIRCLE_SHAPE = 1
player    NO_PLAYER = 0xFF`;

	const stateExample = `            send CONNECT every 1 s
 START ───────────────────────────> CONNECTING
                                      │  CONNECT_REJECT (any reason) ─────> STOPPED
                                      │  CONNECT_ACCEPT(playerId)
                                      v
                                   CONNECTED
   every frame: send INPUT            │  PLAYER_LEFT with my playerId ─────> STOPPED
   every SNAPSHOT: redraw             │  no SNAPSHOT for 2 s ──────────────> STOPPED
                                      │  CONNECT_REJECT NOT_CONNECTED ─────> STOPPED
                                      │  user quits: send DISCONNECT ──────> STOPPED`;

	const bytesExample = `CONNECT with protocol version 3
10 03

CONNECT_ACCEPT, you are player 0
90 00

INPUT of tick 1, keys UP + RIGHT, last snapshot 300
30  00 00 00 01  09  00 00 01 2c

SNAPSHOT of tick 5 with one entity: player 0, green box 103 x 50, centred at (100, 200)
c0  00 00 00 05  00 01
    42 c8 00 00  43 48 00 00  00  00  00  42 ce 00 00  42 48 00 00  00  00  00 e4 30 ff
    x = 100      y = 200      ty  id  va  width = 103  height = 50  sh  do  r  g  b  a`;

	const codeExample = `uint8_t packet[rtype_protocol::INPUT_SIZE] = {};
packet[0] = rtype_protocol::INPUT;
rtype_protocol::writeValue<uint32_t>(&packet[1], tick);
packet[5] = keys;
rtype_protocol::writeValue<uint32_t>(&packet[6], lastSnapshotTick);

uint32_t tick = rtype_protocol::readValue<uint32_t>(&packet[1]);`;

	const checklistExample = `1. open a UDP socket towards the server (default 127.0.0.1:4242)
2. send [0x10, 0x03] every second until CONNECT_ACCEPT or CONNECT_REJECT arrives
3. on CONNECT_ACCEPT, store playerId (0 to 3)
4. every frame (about 60 per second), send a 10 byte INPUT with the keys held
5. for every SNAPSHOT:
     older tick than the one on screen   -> drop it
     newer tick                          -> clear the screen, draw its entities
     same tick                           -> it is another part, add its entities
6. stop on PLAYER_LEFT with your playerId, on CONNECT_REJECT, or after 2 s without SNAPSHOT
7. send [0x11] when the user quits`;
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
	The specification of every UDP packet sent between the clients and the server, version 3
</p>

<p class="max-w-[100ch] leading-7">
	This page is the reference to write a new client for the server. The words
	<strong>MUST</strong>, <strong>SHOULD</strong> and <strong>MAY</strong> are used as in
	<a href="https://www.rfc-editor.org/rfc/rfc2119" class="text-[#ff8a3d] hover:underline">RFC 2119</a>.
	The source of truth is {@render c('src/common/rtype_protocol.hpp')}, shared by the client and the
	server: it defines the packet types, their size and the helpers to read and write numbers.
</p>

<h2 class="mt-9 text-lg font-semibold">1. Transport</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Every message is sent over <strong>UDP</strong>. There is no TCP connection: the connection,
		the inputs and the game state all use the same UDP socket.
	</li>
	<li>
		One datagram holds exactly one packet. A packet MUST NOT be bigger than 1024 bytes
		({@render c('rtype_protocol::Packet')}); bytes after the 1024th are lost.
	</li>
	<li>
		The protocol is binary. Numbers bigger than one byte are sent in <strong>big-endian</strong>,
		the most significant byte first. Floats ({@render c('f32')}) are IEEE 754 single precision,
		copied bit by bit then sent in big-endian.
	</li>
	<li>
		The server listens on {@render c('127.0.0.1:4242')} by default. It must be started with
		{@render c('./r-type_server 0.0.0.0 4242')} to accept clients from other machines.
	</li>
	<li>
		A client is known by its source address (ip and port). A client MUST send every packet from the
		same socket.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">2. The first byte</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The first byte is always the packet type. Its bit 7 gives the direction and bits 6 to 4 group the
	packets by category. A receiver MUST ignore any type it does not know.
</p>
{@render code(firstByteExample)}

<h2 class="mt-9 text-lg font-semibold">3. The packets</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('n')} is the number of entities in the packet. The reserved packets are defined in the
	header for later; a client MUST NOT rely on them.
</p>
{@render code(packetsExample)}

<h3 class="mt-6 font-semibold">CONNECT (0x10), client to server</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	Two bytes: {@render c('0x10')} then the protocol version, {@render c('PROTOCOL_VERSION = 3')}. A
	client MUST send it before any other packet, and SHOULD send it again every second until it gets an
	answer, since UDP can lose it. The server answers with {@render c('CONNECT_ACCEPT')} or
	{@render c('CONNECT_REJECT')}; the rules are in
	<a href="/developer/network-connection" class="text-[#ff8a3d] hover:underline">How connecting works</a>.
</p>

<h3 class="mt-6 font-semibold">INPUT (0x30), client to server</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	Exactly 10 bytes, or the server ignores it. A connected client SHOULD send one per frame, even
	when no key is held: it is also what keeps the client alive (see section 6).
</p>
{@render code(inputLayout)}
<p class="mt-3 max-w-[100ch] leading-7">
	The server only reads the {@render c('keys')} byte. The {@render c('tick')} and
	{@render c('lastSnapshotTick')} are not used yet. While its fire cooldown (0.2 s) is over, the
	server fires a bullet for every input it applies, so {@render c('CHARGE')} has no effect today.
</p>

<h3 class="mt-6 font-semibold">DISCONNECT (0x11), client to server</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	One byte. A client SHOULD send it when the user quits. The server then frees the player id and
	sends {@render c('PLAYER_LEFT')}.
</p>

<h3 class="mt-6 font-semibold">CONNECT_ACCEPT (0x90) and CONNECT_REJECT (0x91), server to client</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	Two bytes. {@render c('CONNECT_ACCEPT')} carries the player id, from {@render c('0')} to
	{@render c('3')}. {@render c('CONNECT_REJECT')} carries the reason. {@render c('NOT_CONNECTED')}
	is also sent to any address that sends something else than {@render c('CONNECT')} without being
	connected, for example an {@render c('INPUT')} after the player was dropped.
</p>

<h3 class="mt-6 font-semibold">PLAYER_LEFT (0x92), server to client</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	Two bytes, the id of the player that left: it disconnected, timed out or was eliminated. It is sent
	to every connected player, and twice to the one that left. When the id is its own, the client is out
	of the game and MUST stop sending. Other players disappear from the next snapshot.
</p>

<h3 class="mt-6 font-semibold">SNAPSHOT (0xC0), server to client</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	The full state of the game, sent to every connected player at each server tick (60 per second).
</p>
{@render code(snapshotLayout)}
<p class="mt-3 max-w-[100ch] leading-7">Each entity record is 25 bytes ({@render c('ENTITY_SIZE')}):</p>
{@render code(entityLayout)}

<h2 class="mt-9 text-lg font-semibold">4. Values</h2>
{@render code(valuesExample)}

<h2 class="mt-9 text-lg font-semibold">5. Snapshot fragments and coordinates</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		A datagram holds at most (1024 - 7) / 25 = <strong>40</strong> entities. A bigger snapshot is
		sent as several {@render c('SNAPSHOT')} packets with the same tick. There is no fragment index
		and no total count.
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
		The snapshot gives the shape, the size, the color and the draw order: a client can draw
		everything with plain shapes. The reference client also picks a texture from the type, the
		variant and the player id (see
		<a href="/user/assets" class="text-[#ff8a3d] hover:underline">Assets and Sprites</a>).
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">6. Session and timing</h2>
{@render code(stateExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>The server runs 60 ticks per second and sends one snapshot per tick.</li>
	<li>
		A client that sends nothing for <strong>2 seconds</strong> is dropped by the server. A client
		SHOULD consider the server gone after 2 seconds without a {@render c('SNAPSHOT')}.
	</li>
	<li>
		The server keeps the 2 newest inputs of each player and applies one per tick. A client SHOULD
		NOT send more than 60 inputs per second, or its oldest inputs are dropped.
	</li>
	<li>
		An address that left or was eliminated cannot join again: it gets
		{@render c('CONNECT_REJECT SERVER_FULL')}. A client MUST use a new socket (a new port) to join
		again.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">7. Malformed packets</h2>
<p class="mt-3 max-w-[100ch] leading-7">What the server does with a packet it does not expect:</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>An empty datagram, or a type it does not know: ignored.</li>
	<li>A {@render c('CONNECT')} that is not 2 bytes, an {@render c('INPUT')} that is not 10 bytes: ignored.</li>
	<li>Any packet but {@render c('CONNECT')} from an unknown address: {@render c('CONNECT_REJECT NOT_CONNECTED')}.</li>
	<li>A {@render c('DISCONNECT')} of any size from a connected address: accepted.</li>
	<li>The unused bits of {@render c('keys')} are not checked.</li>
	<li>An exception while reading a packet is caught: {@render c('Dropped packet: ...')}, and the server goes on.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	A client SHOULD check the size of each packet before reading it and ignore an entity it cannot
	read. Known limit of the reference client: it only checks the minimum size of a packet, and an
	entity type above 4, or a {@render c('PLAYER')} with an id above 3, throws
	{@render c('UnknownComponentType')}, which stops the client.
</p>

<h2 class="mt-9 text-lg font-semibold">8. Examples</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('writeValue')} and {@render c('readValue')} do the big-endian conversion for any number
	type:
</p>
{@render code(codeExample)}
<p class="mt-3 max-w-[100ch] leading-7">Some packets, byte by byte:</p>
{@render code(bytesExample)}

<h2 class="mt-9 text-lg font-semibold">9. Writing a new client</h2>
{@render code(checklistExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Change {@render c('PROTOCOL_VERSION')} whenever the format of a packet changes, so that an old
		client is rejected with {@render c('WRONG_VERSION')} instead of misreading the packets. Update
		this page and {@render c('VERSION')} in {@render c('tests/tests.sh')} at the same time.
	</li>
	<li>
		There is no authentication: the server trusts the source address. See the security part of the
		<a href="/developer/technical-study" class="text-[#ff8a3d] hover:underline">technical study</a>.
	</li>
	<li>
		To see the packets yourself, {@render c('tests/tests.sh')} talks to the server with
		{@render c('/dev/udp')} and decodes every answer.
	</li>
</ul>
