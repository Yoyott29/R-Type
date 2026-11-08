<script>
	const code0 = `git clone https://github.com/EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4
cd G-ING-500-LIL-5-1-rtype-4
./build_rtype.sh`;
	const code1 = `cmake -S . -B build -A x64
cmake --build build --config Release --target r-type_client r-type_server`;
	const code2 = `./r-type_server
./r-type_client`;
	const code3 = `./r-type_server [ip] [port]     # the address to listen on
./r-type_client [ip] [port]     # the address of the server to join
./r-type_server 5000            # one argument is the port: 127.0.0.1:5000
./r-type_client 5000            # joins 127.0.0.1:5000`;
</script>

<svelte:head>
	<title>Getting started - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Getting started</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Build and run R-Type</p>

<p class="mt-3 max-w-[100ch] leading-7">
	R-Type is a reimplementation of the classic shoot 'em up, made with the <a href="/gringo/about" class="text-accent hover:underline">Gringo Engine</a>: an authoritative server runs the game and up to four clients show it. This page gets it running.
</p>

<h2 class="mt-9 text-lg font-semibold">Supported platforms</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Linux: built and tested by the CI (Ubuntu).</li>
	<li>Windows: built with MSVC by the CI, which also checks that the server starts.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Prerequisites</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>CMake 3.28 or newer</li>
	<li>A C++20 compiler: GCC, Clang or MSVC</li>
	<li>Git</li>
	<li>Linux only: the OpenGL and X11 development packages raylib needs. On Ubuntu:<br /><code>sudo apt install build-essential libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev</code></li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	raylib, Asio, miniaudio and nlohmann/json are downloaded by CMake through CPM: you do not install them. The first build needs an internet connection.
</p>

<h2 class="mt-9 text-lg font-semibold">Build</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	On Linux, from the root of the repository:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	On Windows, from a <strong>Developer PowerShell for VS</strong>:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code1}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	Both create <code>r-type_server</code> and <code>r-type_client</code> in the root of the repository. They find their files from their own location, so they can be started from any folder.
</p>

<h2 class="mt-9 text-lg font-semibold">Run on one machine</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Start the server first, then one client per player, each in its own terminal:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code2}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	On Windows: <code>.\r-type_server.exe</code> and <code>.\r-type_client.exe</code>.
</p>

<h2 class="mt-9 text-lg font-semibold">Command line</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Both programs take an optional IPv4 address and port. Without them, they use <code>127.0.0.1:4242</code>.
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code3}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	The client and the server must use the same port. To play on several machines, see <a href="/user/multiplayer" class="text-accent hover:underline">Multiplayer</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">First launch</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Every client opens on the lobby. The first player to join is the host and starts the match with <strong>START</strong>.</li>
	<li>Up to four players; each gets the first free slot and its ship color.</li>
	<li>Read <a href="/user/controls" class="text-accent hover:underline">Controls</a> before playing.</li>
</ul>
