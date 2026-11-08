<script>
	const graphics = {
		head: ['', 'raylib (chosen)', 'SFML', 'SDL2'],
		rows: [
			['Scope', 'window, input, 2D/3D drawing, audio, textures', 'window, input, 2D drawing, audio, network', 'window, input, 2D drawing (audio in add-ons)'],
			['API', 'plain C functions, few concepts', 'C++ classes', 'C functions, lower level'],
			['Build with CPM', 'one CMake project, no system library but OpenGL / X11', 'needs freetype, audio libraries, more CMake options', 'one CMake project, many options'],
			['Windows / Linux', 'both', 'both', 'both'],
			['Downside', 'global state, one window', 'heavier, its network module overlaps Asio', 'more code for the same result']
		]
	};

	const network = {
		head: ['', 'Asio standalone (chosen)', 'Raw sockets', 'ENet', 'Boost.Asio'],
		rows: [
			['Portability', 'one API for Linux and Windows', 'BSD sockets and Winsock differ', 'portable', 'portable'],
			['Weight', 'header-only, downloaded by CPM', 'none', 'small C library', 'needs a part of Boost'],
			['Abstraction', 'endpoint, socket, error_code', 'must be written by hand', 'connections, channels, reliability', 'same as Asio'],
			['Control of the bytes', 'full, we write the protocol', 'full', 'ENet adds its own header and reliability', 'full'],
			['Why not', '', 'more code, two versions to maintain', 'hides the UDP work the project is about', 'Boost is a big dependency for one feature']
		]
	};

	const packageManagers = {
		head: ['', 'CPM (chosen)', 'Conan', 'vcpkg'],
		rows: [
			['Install', 'nothing: a CMake script downloaded at configure time', 'Python tool to install', 'git clone and bootstrap'],
			['Integration', 'inside CMakeLists.txt', 'profiles and a generated toolchain', 'toolchain file'],
			['Binaries', 'built from source (slower first build)', 'prebuilt binaries', 'built from source, binary cache'],
			['CI', 'cache the .cpm-cache folder', 'cache the Conan home', 'cache the binary cache']
		]
	};

	const sync = {
		head: ['', 'Full state, 30 times per second (chosen)', 'Delta snapshots', 'Lockstep (inputs only)'],
		rows: [
			['Packet loss', 'harmless: the next state replaces it, removals are sent 3 times', 'needs acknowledgements and a base snapshot', 'blocks every player until the input arrives'],
			['Bandwidth', 'highest', 'low', 'lowest'],
			['Complexity', 'lowest', 'medium (history, acks)', 'high (determinism on every machine)'],
			['Authority', 'server only', 'server only', 'every client simulates']
		]
	};

	const storage = {
		head: ['', 'Memory only (today)', 'Files (JSON, binary)', 'SQLite', 'Database server'],
		rows: [
			['Persistence', 'none, lost on exit', 'yes', 'yes', 'yes'],
			['Reliability', 'nothing to corrupt', 'a crash during a write can corrupt a file', 'transactions, survives crashes', 'transactions, replication'],
			['Concurrency', 'mutex in the process', 'one writer', 'one writer at a time', 'many writers'],
			['Fits', 'a game session', 'config, levels, saves', 'scores, accounts on one server', 'many servers']
		]
	};

	const threats = {
		head: ['Threat', 'What protects us today', 'Still open'],
		rows: [
			['Malformed packet (too short, too long, unknown kind)', 'datagrams over 1200 bytes, with a wrong protocol id or an unknown kind are dropped; a message that does not decode exactly is dropped as a whole; reserved messages sent the wrong way are ignored', ''],
			['Buffer overflow', 'every read goes through ByteReader, which checks the remaining length and throws, caught per datagram', ''],
			['Input flood from one player', 'the server keeps only the newest input of each player and ignores older ticks', ''],
			['Datagram flood', 'the socket thread keeps at most 1024 datagrams between two frames and drops the rest', 'a flood still uses the bandwidth'],
			['Connection flood', '4 slots, extra clients are rejected as Full, and none are accepted during a match', ''],
			['Spoofed source address', 'none: a player is its ip and port', 'a spoofed Disconnect or input acts for that player'],
			['Reflection', 'a rejection is 6 bytes; the game state is only sent to accepted peers', 'a spoofed ConnectRequest makes the server answer the victim'],
			['Cheating client', 'the server is the only authority: the client only sends keys', ''],
			['Vulnerable dependency', 'raylib, Asio, miniaudio, nlohmann/json and CPM are pinned to a version, CPM.cmake is checked with SHA256', 'no automatic alert when a new CVE is published']
		]
	};
</script>

<svelte:head>
	<title>Technical and comparative study - R-Type docs</title>
</svelte:head>

{#snippet c(text)}<code
		class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]"
		>{text}</code
	>{/snippet}

{#snippet a(href, text)}<a {href} class="text-accent hover:underline">{text}</a>{/snippet}

{#snippet table(t)}
	<div class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26]">
		<table class="w-full text-left text-[14px]">
			<thead class="bg-[#121a17]">
				<tr>
					{#each t.head as h, i (i)}<th scope="col" class="px-3 py-2.5 font-semibold">{h}</th>{/each}
				</tr>
			</thead>
			<tbody>
				{#each t.rows as row, r (r)}
					<tr class="border-t border-[#1e2b26] align-top">
						{#each row as cell, i (i)}
							{#if i === 0}
								<th scope="row" class="px-3 py-2.5 font-medium text-[#d8e4de]">{cell}</th>
							{:else}
								<td class="px-3 py-2.5 text-[#b9c7c0]">{cell}</td>
							{/if}
						{/each}
					</tr>
				{/each}
			</tbody>
		</table>
	</div>
{/snippet}

<h1 class="text-3xl font-semibold">Technical and comparative study</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	Why each technology, algorithm and data structure was chosen, and what the alternatives were
</p>

<p class="max-w-[100ch] leading-7">
	This page explains the choices behind the prototype. Each part compares the option we use with the
	usual alternatives. The limits that are still open are listed too, so they can be picked for the
	next part of the project.
</p>

<h2 class="mt-9 text-lg font-semibold">1. Language: C++20</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>C++ is imposed by the project. It gives full control over memory and data layout, which an ECS needs.</li>
	<li>
		C++20 is fully supported by GCC, Clang and MSVC on both target systems, and the code uses it:
		{@render c('std::span')} to read datagrams without copying them, {@render c('std::bit_cast')} to
		encode floats in the protocol, {@render c('std::jthread')} and {@render c('std::stop_token')} for
		the thread pool, {@render c('std::erase_if')} and {@render c('contains')} on containers, along with
		{@render c('std::variant')} for shapes and {@render c('if constexpr')} in the templates.
	</li>
	<li>
		C++20 modules and C++23 are left out: their support is still uneven between the three compilers.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">2. Build system and package manager: CMake + CPM</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	CMake is the standard build system generator for C++ and works with every IDE and compiler we use.
	For the dependencies we compared the three package managers allowed by the project:
</p>
{@render table(packageManagers)}
<p class="mt-3 max-w-[100ch] leading-7">
	CPM won because a fresh clone only needs CMake and a compiler, on Linux and Windows. The price is
	that raylib is compiled from source; the {@render c('.cpm-cache')} folder (also cached in the CI)
	avoids downloading it again. See {@render a('/developer/cpm', 'How CPM works')}.
</p>

<h2 class="mt-9 text-lg font-semibold">3. Graphics library: raylib</h2>
{@render table(graphics)}
<p class="mt-3 max-w-[100ch] leading-7">
	raylib gives the most with the least code: a window, the keyboard and textured sprites take a few
	calls. Its global state is hidden behind the {@render c('IRenderer')} interface, so the engine never
	depends on it. Game engines (Unity, Godot...) are forbidden by the project.
</p>

<h2 class="mt-9 text-lg font-semibold">4. Networking</h2>
<h3 class="mt-6 font-semibold">Transport: UDP only</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	A game state sent 60 times per second is only useful when it is fresh. With TCP, one lost segment
	blocks every following one until it is sent again (head-of-line blocking). With UDP, a lost
	snapshot is simply replaced by the next one. The project requires UDP for the game, and the
	prototype uses no TCP at all: connecting, leaving and playing all go through the same UDP socket.
	The connection is made reliable by sending {@render c('ConnectRequest')} again every 200 ms, up to 25
	times, until it is answered; a peer that sends nothing for 5 seconds is dropped.
</p>
<h3 class="mt-6 font-semibold">Library: Asio standalone</h3>
{@render table(network)}
<p class="mt-3 max-w-[100ch] leading-7">
	Asio gives one abstraction over BSD sockets and Winsock, without the rest of Boost. Both sides use the
	same {@render c('UdpSocket')}: an Asio {@render c('io_context')} on its own thread receives with
	{@render c('async_receive_from')} into a queue, and {@render c('NetReceiveSystem')} drains it once
	per frame, so the game loop never blocks on the network.
</p>
<h3 class="mt-6 font-semibold">Protocol: custom binary</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	A text protocol (JSON, lines of text) would be easy to read but 3 to 5 times bigger and needs
	parsing. Serialization libraries (Protocol Buffers, FlatBuffers) add a code generator and a
	dependency. A small binary format, written with {@render c('ByteWriter')} / {@render c('ByteReader')}
	in little-endian, is compact and easy to check: a 5 byte header, then messages made of a type, a
	length and a payload. See
	{@render a('/gringo/developer/network-protocol', 'How the protocol works')}.
</p>
<h3 class="mt-6 font-semibold">State synchronisation: full state, 30 times per second</h3>
{@render table(sync)}
<p class="mt-3 max-w-[100ch] leading-7">
	Every other tick, the server sends the state of each replicated entity: its network id, owner,
	prefab name and one block per replicated component (the {@render c('Transform')} is 36 bytes). The
	client creates a copy of the prefab once and then only updates it, so sprites and animations are
	never sent. It makes the server the only authority and survives any packet loss. Cost: about 70
	bytes per entity, so 60 entities are about 4 KB per snapshot, or about 125 KB/s per player.
	Delta snapshots would need a snapshot history on the server and acknowledgements from the clients,
	neither exists yet.
</p>

<h2 class="mt-9 text-lg font-semibold">5. Algorithms and data structures</h2>
<h3 class="mt-6 font-semibold">Existing algorithms and patterns we use</h3>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong>Entity Component System</strong>, recommended by the project. Compared with a class
		hierarchy (Player, Enemy... inheriting from Entity), behaviour comes from the components an entity
		has, so a new kind of entity needs no new class. See {@render a('/gringo/developer/ecs', 'How the ECS works')}.
	</li>
	<li>
		<strong>Sparse set</strong> per component type: a packed {@render c('std::vector')} of components
		(cache friendly, iterated without holes), plus an array from entity id to its indexes. Add is
		O(1), remove is O(1) by moving the last component into the hole.
	</li>
	<li>
		<strong>Bitset signatures</strong>: each entity has a {@render c('std::bitset<32>')} of its
		component types, each system a required set. Matching is one AND per system.
	</li>
	<li>
		<strong>Free list</strong> of entity ids in a {@render c('std::queue')}: create and destroy are
		O(1), and ids stay below {@render c('MAX_ENTITIES')} (4096) so arrays can be indexed directly.
	</li>
	<li>
		<strong>Fixed timestep</strong> on both sides: the game runs exactly 60 ticks per second on a
		clock computed from the start time, so no rounding error builds up, and after a stall at most 15
		late ticks are caught up. The game speed never depends on the CPU speed.
	</li>
	<li>
		<strong>Producer / consumer queue</strong> between the socket I/O thread and
		{@render c('NetReceiveSystem')}, with a mutex, capped at 1024 datagrams.
	</li>
	<li>
		<strong>Dependency graph scheduling</strong>: systems that declare disjoint accesses run
		together on a thread pool, see
		{@render a('/gringo/developer/ecs-systems#parallel-systems', 'Running systems in parallel')}.
	</li>
	<li>
		<strong>Collision tests</strong>: a sort and sweep on x finds the pairs that may touch (run in
		parallel with {@render c('parallelFor')}), then box against box (AABB), circle against circle and
		box against circle tests check them.
	</li>
	<li>
		<strong>Design patterns</strong>: plugin ({@render c('IPlugin')}), bridge / strategy
		({@render c('IRenderer')}), prototype ({@render c('Prefab')}), callbacks
		({@render c('Collider2D::onCollision')}, {@render c('Script')}).
	</li>
</ul>
<h3 class="mt-6 font-semibold">Custom algorithms and why we wrote them</h3>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong>Our own ECS</strong> instead of EnTT or flecs: the project asks for our own engine, and a
		small sparse-set ECS is enough for a few hundred entities. We added
		{@render c('AllowMultiple')}, which lets one entity hold several components of the same type
		(an enemy has two {@render c('Script')}s).
	</li>
	<li>
		<strong>Newest input only</strong>: the server keeps the newest input of each player and ignores
		older ticks, so a late or duplicated packet never moves a ship backwards.
	</li>
	<li>
		<strong>Message packing</strong>: the messages of a frame are packed first-fit into datagrams of
		at most 1200 bytes, below the usual network MTU, so nothing is fragmented by IP.
	</li>
	<li>
		<strong>Ordered state</strong>: each state carries its tick, and the client ignores a state older
		than the newest one it has, so a late packet never moves an entity back.
	</li>
</ul>
<h3 class="mt-6 font-semibold">Limits</h3>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>The sort and sweep is fast when entities are spread along x; a uniform grid would scale better if many entities share the same x.</li>
	<li>Entity ids have no generation counter, so a stored id can point to a newer entity after a destroy.</li>
	<li>Every replicated entity is sent in each snapshot, even when it did not move: delta compression would save bandwidth.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">6. Storage</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The prototype stores nothing: the game state lives in memory for the time of a session, the
	textures are read-only PNG files in {@render c('R-Type/assets/')}, and the settings are command-line
	arguments. There is nothing to back up and nothing that can be corrupted. When scores, accounts or
	levels are added, these are the options:
</p>
{@render table(storage)}
<p class="mt-3 max-w-[100ch] leading-7">
	For one server, SQLite is the best fit for scores and accounts (one file, transactions, no extra
	process). Levels and settings fit in plain files, versioned with the code.
</p>

<h2 class="mt-9 text-lg font-semibold">7. Security</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The server is open on the network, so every packet is treated as untrusted. The main threats and
	what the code does about them:
</p>
{@render table(threats)}
<h3 class="mt-6 font-semibold">How it is checked</h3>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Every pull request to {@render c('main')} runs the CI on Linux and Windows (see
		{@render a('/developer/running-tests', 'Running Tests')}): nothing is merged without it.
	</li>
	<li>
		The network unit tests ({@render c('network_tests')}) feed the decoders malformed data: an
		entity state cut at every possible length or with one byte too many, a prefab name over 64
		bytes, a truncated component block, and states that arrive late or after their despawn. All must
		be rejected without effect.
	</li>
	<li>
		{@render c('tests/tests.sh')} checks that every kind of bad command line argument is refused
		with exit code 84, that a used port fails cleanly, that the server stops cleanly on a signal, and
		that a client killed without warning is dropped by the server. See
		{@render a('/developer/tests-sh', 'How tests.sh works')}.
	</li>
	<li>clang-tidy runs on every build and in the CI to catch bugs such as unchecked conversions, and clang-format checks every file.</li>
	<li>Dependencies are pinned to a version, and the CPM bootstrap is checked against a SHA256 hash.</li>
</ul>
<h3 class="mt-6 font-semibold">Long-term monitoring</h3>
<p class="mt-3 max-w-[100ch] leading-7">In place:</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong>Continuous checks</strong>: the same tests, static analysis and builds run on every pull
		request, so a change that breaks the parsers or the protocol is seen before it is merged.
	</li>
	<li>
		<strong>Runtime logs</strong>: the server prints every connection, departure and timeout
		({@render c('Player 1 connected')}, {@render c('Player 1 timed out')}), and the network plugin
		reports anomalies on stderr, once per kind, without flooding: oversized sends, reserved
		messages of an unknown type, a full player list, the mirror limit reached.
	</li>
	<li>
		<strong>Traffic statistics</strong>: the transport counts, per peer and per second, the packets
		received, missing, late and duplicated, and prints them once per second when
		{@render c('logStats')} is turned on in {@render c('NetworkConfig')}.
	</li>
	<li>
		<strong>Pinned dependencies</strong>: a dependency only changes when its version is changed by
		hand in a reviewed pull request, after reading its release notes and security advisories.
	</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">Next steps, in order of value:</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Enable GitHub Dependabot alerts, to be warned when a pinned dependency gets a security advisory.</li>
	<li>Fuzz the packet and message parsers, and run the tests under AddressSanitizer and UndefinedBehaviorSanitizer in the CI.</li>
	<li>Add a per-address rate limit and a session token in each packet against spoofed addresses.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">8. Accessibility</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	What the game and this documentation offer is described on the
	{@render a('/user/accessibility', 'Accessibility')} page.
</p>
