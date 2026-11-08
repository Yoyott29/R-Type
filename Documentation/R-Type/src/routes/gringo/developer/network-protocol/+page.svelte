<script>
	const code0 = `ConnectRequest   52 47 | 01 | 00 00 | 02 00
                 id      kind seq    version 2

ConnectAccepted  52 47 | 02 | 00 00 | 01 00 00 00
                 id      kind seq    peerId 1

Data             52 47 | 00 | 07 00 | 03 ff | 08 00 | 78 00 00 00 | 28 00 00 00
                 id      kind seq 7   type    length  tick 120      bits 0x28`;
	const code1 = `f0 00 00 00                     tick 240
03 00 00 00                     netId 3
01 00 00 00                     owner 1
0a 00 50 6c 61 79 65 72 42 6c 75 65    prefab "PlayerBlue" (length 10)
01                              1 block
bb ab 82 ad 24 00               codecId 0xAD82ABBB (Transform), length 36
00 00 70 44 00 00 07 44 00 00 00 00    x 960, y 540, z 0
00 00 80 3f 00 00 80 3f 00 00 80 3f    scale 1, 1, 1
00 00 00 00 00 00 00 00 00 00 00 00    rotation 0, 0, 0`;
</script>

<svelte:head>
	<title>How the protocol works - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">How the protocol works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">The wire format of the Network plugin, version 2: enough to write a client or a server</p>

<p class="mt-3 max-w-[100ch] leading-7">
	This page describes every byte the Network plugin sends, the order of the exchanges and the timings, the way an RFC would. It is the same for every Gringo game; what a game puts inside (its prefabs and input bits) is described by the game, for R-Type in <a href="/developer/protocol" class="text-accent hover:underline">R-Type network protocol</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">1. Conventions</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Transport: <strong>UDP only</strong>, one socket per side. One UDP datagram is one packet.</li>
	<li>Every integer is <strong>little-endian</strong>. <code>u8</code>, <code>u16</code>, <code>u32</code> are unsigned, <code>i32</code> signed, <code>f32</code> an IEEE 754 float.</li>
	<li><code>str</code> is a <code>u16</code> length followed by that many bytes, without a terminating zero.</li>
	<li>A datagram is at most <strong>1200 bytes</strong>, below the usual network MTU.</li>
	<li>The protocol is unreliable: any packet can be lost, duplicated or reordered. The design makes that harmless instead of retransmitting.</li>
	<li>Anything that does not match this page exactly is <strong>dropped silently</strong>: wrong id, unknown kind, wrong size, short or trailing bytes.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">2. Packet header</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Every packet starts with the same 5 bytes:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Offset</th><th class="px-4 py-3">Field</th><th class="px-4 py-3">Type</th><th class="px-4 py-3">Value</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">0</td><td class="px-4 py-3 align-top">protocolId</td><td class="px-4 py-3 align-top">u16</td><td class="px-4 py-3 align-top"><code>0x4752</code> (bytes <code>52 47</code>)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">2</td><td class="px-4 py-3 align-top">kind</td><td class="px-4 py-3 align-top">u8</td><td class="px-4 py-3 align-top">see below</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">seq</td><td class="px-4 py-3 align-top">u16</td><td class="px-4 py-3 align-top">sequence number of this packet, see section 4</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">3. Packet kinds</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">kind</th><th class="px-4 py-3">Name</th><th class="px-4 py-3">Sent by</th><th class="px-4 py-3">Body after the header</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">0</td><td class="px-4 py-3 align-top">Data</td><td class="px-4 py-3 align-top">both</td><td class="px-4 py-3 align-top">zero or more messages, see section 5</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">1</td><td class="px-4 py-3 align-top">ConnectRequest</td><td class="px-4 py-3 align-top">client</td><td class="px-4 py-3 align-top"><code>version u16</code>, currently <code>2</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">2</td><td class="px-4 py-3 align-top">ConnectAccepted</td><td class="px-4 py-3 align-top">server</td><td class="px-4 py-3 align-top"><code>peerId u32</code>: the id the server gives this client</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">ConnectRejected</td><td class="px-4 py-3 align-top">server</td><td class="px-4 py-3 align-top"><code>reason u8</code>: 0 Full, 1 VersionMismatch</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">4</td><td class="px-4 py-3 align-top">KeepAlive</td><td class="px-4 py-3 align-top">both</td><td class="px-4 py-3 align-top">empty</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">5</td><td class="px-4 py-3 align-top">Disconnect</td><td class="px-4 py-3 align-top">both</td><td class="px-4 py-3 align-top">empty</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	Control packets have an exact size: a ConnectRequest is 7 bytes, a ConnectAccepted 9, a ConnectRejected 6, a KeepAlive and a Disconnect 5.
</p>

<h2 class="mt-9 text-lg font-semibold">4. A session</h2>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Connecting</h3>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The client sends a <strong>ConnectRequest</strong> every 200 ms, at most 25 times (about 5 seconds), until it gets an answer.</li>
	<li>The server answers <strong>ConnectAccepted</strong> with the client's peer id. Peer ids start at 1 and are never reused while the server runs. To a client, the server is always peer 0.</li>
	<li>The server answers <strong>ConnectRejected</strong> when the version differs (VersionMismatch), when its slots are full or when the game does not accept players right now (Full). The client then stops trying.</li>
	<li>If the ConnectAccepted is lost, the client sends its ConnectRequest again and the server sends the ConnectAccepted again.</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Staying connected</h3>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Each side sends <strong>something at least every 250 ms</strong>: its Data packets, or a KeepAlive when it has nothing to say.</li>
	<li>A side that receives <strong>nothing valid for 5 seconds</strong> considers the other one gone (timeout).</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Sequence numbers</h3>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Each side numbers the packets it sends on a connection: 0, 1, 2... wrapping after 65535. Every packet, whatever its kind, takes the next number.</li>
	<li>The receiver keeps the newest number it saw and which of the 32 before it arrived. It drops a packet already received (duplicate) or more than 32 behind the newest (too late). A late packet inside the window is accepted.</li>
	<li>Sequence numbers are only used to drop duplicates and count losses: nothing is retransmitted.</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Leaving</h3>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>A side that leaves sends <strong>Disconnect 3 times</strong>, in case some are lost.</li>
	<li>When a client leaves or times out, the server tells the other clients with a PeerLeft message.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">5. Data packets and messages</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The body of a Data packet is a list of messages, one after the other, until the end of the datagram. Each message is:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Field</th><th class="px-4 py-3">Type</th><th class="px-4 py-3">Meaning</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">type</td><td class="px-4 py-3 align-top">u16</td><td class="px-4 py-3 align-top">what the message is</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">length</td><td class="px-4 py-3 align-top">u16</td><td class="px-4 py-3 align-top">size of the payload, at most 1191</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">payload</td><td class="px-4 py-3 align-top">length bytes</td><td class="px-4 py-3 align-top">depends on the type</td></tr>
		</tbody>
	</table>
</div>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Types below <code>0xFF00</code> belong to the game and are given to it untouched. Types from <code>0xFF00</code> are <strong>reserved</strong> to the engine (section 6).</li>
	<li>A reserved message of an unknown type, or sent the wrong way (for example an InputState sent by the server), is dropped.</li>
	<li>A message whose payload does not decode exactly (short, or with bytes left over) is dropped as a whole; the other messages of the packet are still read.</li>
	<li>A sender packs as many messages as fit in one datagram, then starts another one.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">6. Reserved messages</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">type</th><th class="px-4 py-3">Name</th><th class="px-4 py-3">Direction</th><th class="px-4 py-3">Payload</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>0xFF01</code></td><td class="px-4 py-3 align-top">EntityState</td><td class="px-4 py-3 align-top">server to client</td><td class="px-4 py-3 align-top"><code>tick u32, netId u32, owner u32, prefab str, count u8</code>, then <code>count</code> blocks of <code>codecId u32, length u16, length bytes</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>0xFF02</code></td><td class="px-4 py-3 align-top">Despawn</td><td class="px-4 py-3 align-top">server to client</td><td class="px-4 py-3 align-top"><code>tick u32, count u16</code>, then <code>count</code> times <code>netId u32</code> (at most 296 per message)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>0xFF03</code></td><td class="px-4 py-3 align-top">InputState</td><td class="px-4 py-3 align-top">client to server</td><td class="px-4 py-3 align-top"><code>tick u32, bits u32</code>: bit <em>i</em> set means action <em>i</em> is held</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>0xFF04</code></td><td class="px-4 py-3 align-top">PeerLeft</td><td class="px-4 py-3 align-top">server to client</td><td class="px-4 py-3 align-top"><code>peer u32, reason u8</code>: 0 Timeout, 1 Remote (it left), 2 ConnectFailed, 3 Full, 4 VersionMismatch</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>0xFF05</code></td><td class="px-4 py-3 align-top">ScoreUpdate</td><td class="px-4 py-3 align-top">server to client</td><td class="px-4 py-3 align-top"><code>tick u32, score i32</code>: the game's score</td></tr>
		</tbody>
	</table>
</div>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">EntityState</h3>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>netId</code> identifies the entity for as long as it exists; it is never 0 and never reused.</li>
	<li><code>owner</code> is the peer id of the player controlling it, or <code>0xFFFFFFFF</code> for an entity owned by nobody. A client recognises its own player by <code>owner == its peerId</code>.</li>
	<li><code>prefab</code> (at most 64 bytes) is the name of the prefab to create: the client builds its own copy the first time it sees a <code>netId</code>, then only updates it. Its look (sprite, animation) comes from the prefab and is never sent.</li>
	<li>Each block holds one replicated component. <code>codecId</code> is the <strong>32-bit FNV-1a hash</strong> of the component's name (offset basis 2166136261, prime 16777619, over the name's bytes). A block with an unknown codec id can be skipped thanks to its length.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	The components replicated today:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Component</th><th class="px-4 py-3">codecId</th><th class="px-4 py-3">Bytes</th><th class="px-4 py-3">Layout</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Transform</code></td><td class="px-4 py-3 align-top"><code>0xAD82ABBB</code></td><td class="px-4 py-3 align-top">36</td><td class="px-4 py-3 align-top"><code>x, y, z, scaleX, scaleY, scaleZ, rotationX, rotationY, rotationZ</code>, all f32 (rotations in degrees)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>RatStatus</code> (TrackNTrap)</td><td class="px-4 py-3 align-top"><code>0xC22B40BA</code></td><td class="px-4 py-3 align-top">19</td><td class="px-4 py-3 align-top"><code>lap i32, trapsLeft i32, hits i32, respawns i32, boosted u8, moving u8, grounded u8</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>RaceStatus</code> (TrackNTrap)</td><td class="px-4 py-3 align-top"><code>0xD26FB15A</code></td><td class="px-4 py-3 align-top">20</td><td class="px-4 py-3 align-top"><code>phase i32, countdown f32, time f32, winner i32, laps i32</code></td></tr>
		</tbody>
	</table>
</div>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Timing and ordering</h3>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The server runs 60 ticks per second and sends a <strong>snapshot every 2 ticks</strong> (30 per second): one EntityState per replicated entity, then the Despawns, then a ScoreUpdate.</li>
	<li>A removed entity is listed in the Despawns of <strong>3 snapshots in a row</strong>, so one lost packet is not enough to miss it. A client can also remove a copy the server stopped sending for 30 ticks.</li>
	<li>Every EntityState, Despawn and ScoreUpdate carries the server tick. A client ignores one older than the newest tick it has already applied.</li>
	<li>A client sends one <strong>InputState per tick</strong> of its own clock. The server keeps the newest one of each player (by tick) and applies it until a newer one arrives.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">7. Example bytes</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A client connecting, then holding actions 3 and 5 (bits <code>0x28</code>) at its tick 120:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	An EntityState payload: entity 3, owned by peer 1, prefab <code>PlayerBlue</code>, at (960, 540):
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code1}</code></pre>

<h2 class="mt-9 text-lg font-semibold">8. Robustness rules for an implementation</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Check the size of a datagram (5 to 1200 bytes), the protocol id and the kind before reading anything.</li>
	<li>Read every field with a bounds check; treat a short read as a malformed message, never as a crash.</li>
	<li>Ignore datagrams from an address you have no session with, except a ConnectRequest on the server.</li>
	<li>Never trust a client: the server only takes its input bits, and decides everything else.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	For R-Type, the input bits, the prefab names and a working client written from this page are in <a href="/developer/protocol" class="text-accent hover:underline">R-Type network protocol</a>.
</p>
