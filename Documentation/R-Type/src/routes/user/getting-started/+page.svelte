<script>
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';

	const winTools = `winget install Kitware.CMake
winget install Git.Git`;
	const winCheck = `cl
cmake --version
git --version`;
	const winBuild = `git clone https://github.com/EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4
cd G-ING-500-LIL-5-1-rtype-4
cmake -S . -B build -A x64
cmake --build build --config Release --target r-type_client r-type_server --parallel`;
	const winRun = `.\\r-type_server.exe
.\\r-type_client.exe`;
	const winArgs = `.\\r-type_server.exe [ip] [port]   # the address to listen on
.\\r-type_client.exe [ip] [port]   # the address of the server to join
.\\r-type_server.exe 5000          # one argument is the port: 127.0.0.1:5000
.\\r-type_client.exe 5000          # joins 127.0.0.1:5000`;

	const linuxPackages = `sudo apt update
sudo apt install build-essential cmake git \\
  libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev \\
  libxcursor-dev libxinerama-dev`;
	const linuxBuild = `git clone https://github.com/EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4
cd G-ING-500-LIL-5-1-rtype-4
./build_rtype.sh`;
	const linuxRun = `./r-type_server
./r-type_client`;
	const linuxArgs = `./r-type_server [ip] [port]     # the address to listen on
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
	<li>Windows: built with MSVC by the CI, which also checks that the server starts.</li>
	<li>Linux: built and tested by the CI (Ubuntu).</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	raylib, Asio, miniaudio and nlohmann/json are downloaded by CMake through CPM: you do not install them. The first build needs an internet connection.
</p>

<h2 class="mt-9 text-lg font-semibold">Prerequisites</h2>

<Platform os="windows">
	<ol class="mt-3 list-decimal space-y-1.5 pl-6 leading-7">
		<li>
			Install the <a href="https://visualstudio.microsoft.com/downloads/" class="text-accent hover:underline">Visual Studio Build Tools</a> (the full Visual Studio IDE also works). In <strong>Workloads</strong>, select <strong>Desktop development with C++</strong>, and keep <strong>MSVC C++ build tools</strong>, the <strong>Windows 10/11 SDK</strong> and <strong>C++ CMake tools for Windows</strong> selected.
		</li>
		<li>Install CMake 3.28 or newer and Git if the Build Tools did not install them. From PowerShell:</li>
	</ol>
	<pre class={pre}><code>{winTools}</code></pre>
	<p class="mt-3 leading-7">
		Every command on this page runs in a <strong>Developer PowerShell for VS 2022</strong> (from the Start menu). A regular PowerShell window does not know the MSVC compiler. Check that everything is found:
	</p>
	<pre class={pre}><code>{winCheck}</code></pre>
	<p class="mt-3 leading-7">
		<code>cl</code> prints the Microsoft C/C++ compiler version. If it is not recognised, you are not in the developer terminal.
	</p>
</Platform>

<Platform os="linux">
	<p class="mt-3 leading-7">
		A C++20 compiler (GCC or Clang), CMake 3.28 or newer, Git, and the OpenGL and X11 development packages raylib needs. On Ubuntu/Debian:
	</p>
	<pre class={pre}><code>{linuxPackages}</code></pre>
	<p class="mt-3 leading-7">
		Check <code>cmake --version</code>: if your distribution ships an older CMake, install a recent one with <code>sudo snap install cmake --classic</code> or from <a href="https://cmake.org/download/" class="text-accent hover:underline">cmake.org</a>.
	</p>
</Platform>

<h2 class="mt-9 text-lg font-semibold">Build</h2>

<Platform os="windows">
	<p class="mt-3 leading-7">
		The <code>.sh</code> scripts are for Linux: on Windows, call CMake directly from the developer terminal.
	</p>
	<pre class={pre}><code>{winBuild}</code></pre>
	<p class="mt-3 leading-7">
		This creates <code>r-type_server.exe</code> and <code>r-type_client.exe</code> in the root of the repository, with <code>gringo_engine.dll</code> next to them. The engine plugins and renderer are written as <code>.dll</code> files in <code>Gringo Engine\plugins\</code> and <code>Gringo Engine\renderers\</code>.
	</p>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{linuxBuild}</code></pre>
	<p class="mt-3 leading-7">
		<code>build_rtype.sh</code> configures the root CMake project and builds both programs in parallel. It creates <code>r-type_server</code> and <code>r-type_client</code> in the root of the repository. The engine library, <code>libgringo_engine.so</code>, and the plugins (<code>.so</code> files) are written in <code>Gringo Engine/</code>.
	</p>
</Platform>

<p class="mt-5 max-w-[100ch] leading-7">
	The programs find their files from their own location, so they can be started from any folder, as long as they stay in the repository.
</p>

<h2 class="mt-9 text-lg font-semibold">Run on one machine</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Start the server first, then one client per player, each in its own terminal. A client tries to connect for about 5 seconds, then quits if no server answers.
</p>

<Platform os="windows">
	<pre class={pre}><code>{winRun}</code></pre>
	<p class="mt-3 leading-7">
		These run from any PowerShell window, not only the developer one. The first time the server starts, Windows Firewall may ask whether it can use the network: allow it to play with other machines.
	</p>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{linuxRun}</code></pre>
</Platform>

<h2 class="mt-9 text-lg font-semibold">Command line</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Both programs take an optional IPv4 address and port. Without them, they use <code>127.0.0.1:4242</code>.
</p>

<Platform os="windows">
	<pre class={pre}><code>{winArgs}</code></pre>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{linuxArgs}</code></pre>
</Platform>

<p class="mt-5 max-w-[100ch] leading-7">
	The client and the server must use the same port. To play on several machines, see <a href="/user/multiplayer" class="text-accent hover:underline">Multiplayer</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">First launch</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Every client opens on the lobby. The first player to join is the host and starts the match with <strong>START</strong>.</li>
	<li>Up to four players; each gets the first free slot and its ship color.</li>
	<li>Read <a href="/user/controls" class="text-accent hover:underline">Controls</a> before playing.</li>
</ul>
