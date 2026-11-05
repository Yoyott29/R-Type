<script>
	const layersExample = `UDP datagram (at most 1200 bytes, little-endian)
└── packet header      protocolId u16 = 0x4752, kind u8, seq u16           (5 bytes)
    └── Data body      messages: type u16, len u16, payload (at most 1191)  (4 + len bytes)`;

	const kindsExample = `kind  name              body
0     Data              zero or more messages
1     ConnectRequest    version u16 (= 2)
2     ConnectAccepted   peerId u32
3     ConnectRejected   reason u8: 0 Full, 1 VersionMismatch
4     KeepAlive         empty
5     Disconnect        empty`;

	const reservedExample = `type    name         direction         payload
0xFF01  EntityState  server -> client  tick u32, netId u32, owner u32, prefab str (<= 64),
                                       n u8, n x (codecId u32, len u16, bytes)
0xFF02  Despawn      server -> client  tick u32, n u16, n x netId u32 (<= 296 per message)
0xFF03  InputState   client -> server  tick u32, bits u32 (bit i = action i)
0xFF04  PeerLeft     server -> client  peer u32, reason u8`;

	const timingExample = `handshake   ConnectRequest every 200 ms, 25 tries (about 5 s)
keep-alive  some packet at least every 250 ms
timeout     nothing accepted for 5 s
disconnect  Disconnect sent 3 times
seq         drop duplicates and packets more than 32 behind
reasons     0 Timeout, 1 Remote, 2 ConnectFailed, 3 Full, 4 VersionMismatch`;
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
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">The wire format of the Network plugin, version 2</p>

<p class="max-w-[100ch] leading-7">
	Every integer is little-endian; {@render c('str')} is a {@render c('u16')} length then the bytes.
	The protocol is unreliable: any packet can be lost, duplicated or reordered.
</p>

<h2 class="mt-9 text-lg font-semibold">Layers</h2>
{@render code(layersExample)}

<h2 class="mt-9 text-lg font-semibold">Packet kinds</h2>
{@render code(kindsExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	A server gives peer ids from 1, never reused; to a client the server is always peer 0. A
	malformed datagram (wrong id, size or kind) is dropped silently.
</p>

<h2 class="mt-9 text-lg font-semibold">Connection timing</h2>
{@render code(timingExample)}

<h2 class="mt-9 text-lg font-semibold">Messages</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Types below {@render c('0xFF00')} belong to the game and are delivered untouched. Types from
	{@render c('0xFF00')} are reserved: a game cannot send them, and an unknown one or one sent the
	wrong way is dropped. A message that does not decode exactly (short or trailing bytes) is dropped
	as a whole.
</p>
{@render code(reservedExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('codecId')} is the FNV-1a 32-bit hash of the component's registered name
	({@render c('Transform')} = {@render c('0xAD82ABBB')}, {@render c('f32 x, y, z, scaleX, scaleY, rotation')}).
</p>
