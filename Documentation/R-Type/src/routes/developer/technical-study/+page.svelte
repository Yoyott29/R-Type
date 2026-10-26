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
		head: ['', 'Full snapshot every tick (chosen)', 'Delta snapshots', 'Lockstep (inputs only)'],
		rows: [
			['Packet loss', 'harmless: the next tick replaces everything', 'needs acknowledgements and a base snapshot', 'blocks every player until the input arrives'],
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
			['Malformed packet (too short, too long, unknown type)', 'fixed 1024 byte buffer, exact sizes for CONNECT and INPUT, unknown types ignored, exceptions caught per packet on the server', 'the client throws on an unknown entity type or player id and stops'],
			['Buffer overflow', 'every read is bounded by the received length and the std::array size', ''],
			['Input flood from one player', 'the InputQueue keeps 2 inputs, the server applies 1 per tick', ''],
			['CONNECT flood', '4 slots, extra clients get SERVER_FULL', 'the set of eliminated addresses grows with every new address'],
			['Spoofed source address', 'none: a player is its ip and port', 'a spoofed DISCONNECT or INPUT acts for that player'],
			['Reflection', 'answers to CONNECT are 2 bytes', 'a spoofed CONNECT makes the server send snapshots to the victim for 2 s'],
			['Cheating client', 'the server is the only authority: the client only sends keys', ''],
			['Vulnerable dependency', 'raylib, Asio and CPM are pinned, CPM.cmake is checked with SHA256', 'no automatic alert when a new CVE is published']
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

{#snippet a(href, text)}<a {href} class="text-[#ff8a3d] hover:underline">{text}</a>{/snippet}

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

<h2 class="mt-9 text-lg font-semibold">1. Language: C++17</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>C++ is imposed by the project. It gives full control over memory and data layout, which an ECS needs.</li>
	<li>
		C++17 is the newest standard fully supported by GCC, Clang and MSVC on both target systems. The
		code uses its features directly: {@render c('std::variant')} and {@render c('std::visit')} for
		shapes, {@render c('if constexpr')} in the protocol helpers, {@render c('std::optional')} for
		input queues, structured bindings, and {@render c('std::from_chars')} to parse the port.
	</li>
	<li>
		C++20 (concepts, modules, {@render c('std::span')}) would help, but nothing in the prototype needs
		it, and module support is still uneven between compilers.
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
	The connection is made reliable by sending {@render c('CONNECT')} again every second until it is
	answered.
</p>
<h3 class="mt-6 font-semibold">Library: Asio standalone</h3>
{@render table(network)}
<p class="mt-3 max-w-[100ch] leading-7">
	Asio gives one abstraction over BSD sockets and Winsock, without the rest of Boost. The server uses
	a blocking {@render c('receive_from')} in its own thread; the client uses a non-blocking socket read
	once per frame, so neither side needs an event loop.
</p>
<h3 class="mt-6 font-semibold">Protocol: custom binary</h3>
<p class="mt-2 max-w-[100ch] leading-7">
	A text protocol (JSON, lines of text) would be easy to read but 3 to 5 times bigger and needs
	parsing. Serialization libraries (Protocol Buffers, FlatBuffers) add a code generator and a
	dependency. A small binary format, written with {@render c('writeValue')} / {@render c('readValue')}
	in big-endian, is compact, has a fixed size per packet type, and is easy to check. See
	{@render a('/developer/network-protocol', 'How the protocol works')}.
</p>
<h3 class="mt-6 font-semibold">State synchronisation: full snapshots</h3>
{@render table(sync)}
<p class="mt-3 max-w-[100ch] leading-7">
	Full snapshots make the server the only authority and survive any packet loss, which is what a
	first prototype needs. Cost: each entity is 11 bytes (position, type, player id, variant: the
	client picks the look), so 60 entities are about 0.7 KB per tick, or about 40 KB/s per player at
	60 ticks per second. Delta snapshots would need a snapshot history on the server and an
	acknowledged tick from the clients, neither exists yet.
</p>

<h2 class="mt-9 text-lg font-semibold">5. Algorithms and data structures</h2>
<h3 class="mt-6 font-semibold">Existing algorithms and patterns we use</h3>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong>Entity Component System</strong>, recommended by the project. Compared with a class
		hierarchy (Player, Enemy... inheriting from Entity), behaviour comes from the components an entity
		has, so a new kind of entity needs no new class. See {@render a('/developer/ecs', 'How the ECS works')}.
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
		<strong>Fixed timestep with an accumulator</strong> on the client ("Fix your timestep"), and a
		tick clock computed from the start time on the server, so no rounding error builds up and the
		game speed never depends on the CPU speed.
	</li>
	<li>
		<strong>Producer / consumer queue</strong> between the network thread and the game thread, with a
		mutex.
	</li>
	<li>
		<strong>Collision tests</strong>: box against box (AABB), circle against circle (squared
		distances, no square root) and box against circle (closest point), checked pair by pair.
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
		<strong>Input queue that keeps the 2 newest inputs</strong>: when a client sends faster than the
		server ticks, old inputs are dropped instead of piling up, which keeps the input lag under two
		ticks.
	</li>
	<li>
		<strong>Snapshot fragments with the same tick</strong>: a snapshot bigger than one datagram is cut
		into parts of 40 entities. The client keeps only the newest tick, so after a freeze it does not
		replay old frames.
	</li>
	<li>
		<strong>Snapshot history as a ring buffer</strong> of 64 entries indexed by
		{@render c('tick % 64')}: no allocation per tick, a fixed amount of memory.
	</li>
</ul>
<h3 class="mt-6 font-semibold">Limits</h3>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Collision is O(n²). A uniform grid would be the next step if the number of entities grows.</li>
	<li>Entity ids have no generation counter, so a stored id can point to a newer entity after a destroy.</li>
	<li>The client rebuilds every entity on each snapshot. A network id per entity would let it update them instead.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">6. Storage</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The prototype stores nothing: the game state lives in memory for the time of a session, the
	textures are read-only PNG files in {@render c('Assets/')}, and the settings are command-line
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
		{@render c('tests/tests.sh')} has a Robustness section run on every pull request to main: unknown
		packets, one-byte packets, huge packets, garbage floods, input floods, clients that vanish, many
		sessions. See {@render a('/developer/tests-sh', 'How tests.sh works')}.
	</li>
	<li>clang-tidy runs on every build and in the CI to catch bugs such as unchecked conversions.</li>
	<li>Dependencies are pinned to a version, and the CPM bootstrap is checked against a SHA256 hash.</li>
</ul>
<h3 class="mt-6 font-semibold">Long-term monitoring (not in place yet)</h3>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Watch the raylib and Asio security advisories, or enable GitHub Dependabot alerts, before updating the pinned tags.</li>
	<li>Add a CI build with AddressSanitizer and UndefinedBehaviorSanitizer while the tests run.</li>
	<li>Fuzz the packet parsers of the client and the server.</li>
	<li>Add a per-address rate limit, a session token in each packet against spoofing, and a cap on the eliminated addresses.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">8. Accessibility</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	What the game and this documentation offer, and what is missing, is described on the
	{@render a('/user/accessibility', 'Accessibility')} page.
</p>
