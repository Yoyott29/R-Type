<script>
	const packetsExample = `type            byte   content                                            size       direction
CONNECT         0x10   [0x10, version]                                    2          client -> server
DISCONNECT      0x11   [0x11]                                             1          client -> server
INPUT           0x30   [0x30, tick (u32), keys, lastSnapshotTick (u32)]   10         client -> server
CONNECT_ACCEPT  0x90   [0x90, playerId]                                   2          server -> client
CONNECT_REJECT  0x91   [0x91, reason]                                     2          server -> client
PLAYER_LEFT     0x92   [0x92, playerId]                                   2          server -> client
SNAPSHOT        0xC0   [0xC0, tick (u32), count (u16), entities...]       7 + 9 * n  server -> client`;

	const valuesExample = `keys     UP = 0x01, DOWN = 0x02, LEFT = 0x04, RIGHT = 0x08, CHARGE = 0x10 (fire)
reason   SERVER_FULL = 0x01, WRONG_VERSION = 0x02, NOT_CONNECTED = 0x04
entity   [x (f32), y (f32), type (u8)]
type     PLAYER = 0, ENEMY = 1, BULLET = 2, OBSTACLE = 3`;

	const bytesExample = `INPUT of tick 1, keys UP + RIGHT, last snapshot 300
30  00 00 00 01  09  00 00 01 2c

SNAPSHOT of tick 5 with one player at (100, 200)
c0  00 00 00 05  00 01  42 c8 00 00  43 48 00 00  00`;

	const codeExample = `uint8_t packet[rtype_protocol::INPUT_SIZE] = {};
packet[0] = rtype_protocol::INPUT;
rtype_protocol::writeValue<uint32_t>(&packet[1], tick);
packet[5] = keys;
rtype_protocol::writeValue<uint32_t>(&packet[6], lastSnapshotTick);

uint32_t tick = rtype_protocol::readValue<uint32_t>(&packet[1]);`;
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
	The format of every UDP packet sent between the clients and the server
</p>

<p class="max-w-[100ch] leading-7">
	The client and the server share one file, {@render c('src/common/rtype_protocol.hpp')}. It
	defines the packet types, their size and the helpers to read and write numbers. A packet is a
	{@render c('Packet')}, an array of at most 1024 bytes, and each UDP datagram holds exactly one
	packet.
</p>

<h2 class="mt-9 text-lg font-semibold">The first byte</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The first byte is always the packet type. Its bit 7 gives the direction: it is
	{@render c('0')} for packets sent by the client and {@render c('1')}
	({@render c('SERVER_TO_CLIENT_BIT')}) for packets sent by the server. The receiver reads this byte
	and ignores any type it does not know.
</p>

<h2 class="mt-9 text-lg font-semibold">The packets</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	These are the packets used today. A packet with the wrong size is ignored, except for an unknown
	client, which gets {@render c('NOT_CONNECTED')} (see
	<a href="/developer/network-connection" class="text-[#ff8a3d] hover:underline">How connecting works</a>).
</p>
{@render code(packetsExample)}
{@render code(valuesExample)}

<h2 class="mt-9 text-lg font-semibold">Numbers on the wire</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Numbers bigger than one byte are sent in big-endian, the most significant byte first.
	{@render c('writeValue')} and {@render c('readValue')} do the conversion for any number type.
	Floats are copied bit by bit, so a position is received exactly as it was sent.
</p>
{@render code(codeExample)}
<p class="mt-3 max-w-[100ch] leading-7">The same packets, byte by byte:</p>
{@render code(bytesExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('PROTOCOL_VERSION')} is {@render c('1')}. Change it whenever the format of a packet
		changes, so that an old client is rejected instead of misreading the packets.
	</li>
	<li>
		{@render c('GAME_START')}, {@render c('GAME_STARTED')}, {@render c('DELTA')} and the reason
		{@render c('GAME_ALREADY_STARTED')} are defined for later. The server never sends them yet.
	</li>
	<li>
		The server only reads the keys of an {@render c('INPUT')}: the tick and the snapshot
		acknowledgement are not used yet.
	</li>
	<li>
		To see the packets yourself, {@render c('tests/tests.sh')} talks to the server with
		{@render c('/dev/udp')} and decodes every answer.
	</li>
</ul>
