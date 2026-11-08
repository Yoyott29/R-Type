<script>
	import Platform from '$lib/components/Platform.svelte';
</script>

<svelte:head>
	<title>Troubleshooting - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Troubleshooting</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Common launch and gameplay problems</p>

<h2 class="mt-9 text-lg font-semibold">The lobby stays empty</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The client opened but all four slots say <code>EMPTY</code>: it is not connected. The client tries for about 5 seconds, then stays in the lobby without retrying. Check that:
</p>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>the server is running, and was started before the client;</li>
	<li>the client uses the same address and port as the server;</li>
	<li>for another machine, the server was started on <code>0.0.0.0</code> and the firewall allows UDP on that port;</li>
	<li>the game is not full (four players) and no match is running;</li>
	<li>both were built from the same version.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	Then start the client again.
</p>

<h2 class="mt-9 text-lg font-semibold">Messages</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	An error stops the program with <code>Runtime error: ...</code> and the exit code 84.
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Message</th><th class="px-4 py-3">What it means</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Invalid port: ...</code></td><td class="px-4 py-3 align-top">The port is not a number between 1 and 65535.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Invalid ip: ...</code></td><td class="px-4 py-3 align-top">The address is not an IPv4 address such as <code>192.168.1.20</code>.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Usage: [ip] port</code></td><td class="px-4 py-3 align-top">Too many arguments.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>bind: Address already in use</code></td><td class="px-4 py-3 align-top">Another program (often another server) uses that port. Stop it, or use another port for the server and the clients.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Could not open the window</code></td><td class="px-4 py-3 align-top">No display is available (SSH session, missing graphics drivers).</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Could not find R-Type.gringo next to the executable ...</code></td><td class="px-4 py-3 align-top">The executable was moved away from the repository. Run it from the root of the repository, where it was built.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Could not load the plugin ...</code></td><td class="px-4 py-3 align-top">The engine plugins are missing or were not built: build again (on Windows with the CMake commands of <a href="/user/getting-started" class="text-accent hover:underline">Getting started</a>, on Linux with <code>./build_rtype.sh</code>).</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>File not found: ...</code></td><td class="px-4 py-3 align-top">An image, a sound or a model is missing from <code>R-Type/assets/</code>.</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	The server also prints <code>Player 1 connected</code>, <code>Player 1 left</code> and <code>Player 1 timed out</code>. Lines starting with <code>NetworkPlugin:</code> are warnings about the network; the game goes on.
</p>

<h2 class="mt-9 text-lg font-semibold">Build failures</h2>

<Platform os="windows">
	<ul class="mt-3 list-disc space-y-1.5 pl-6 leading-7">
		<li><code>cl</code> is not recognised, or CMake finds no C++ compiler: open a <strong>Developer PowerShell for VS 2022</strong> instead of a regular PowerShell window.</li>
		<li><code>./build_rtype.sh</code> or <code>build_engine.sh</code> does nothing or fails: they are Bash scripts. Use the <code>cmake</code> commands from <a href="/user/getting-started" class="text-accent hover:underline">Getting started</a>.</li>
		<li>Building a game from the Gringo editor (<strong>Build</strong> or <code>Gringo.exe --build</code>) fails with <code>'cmake' is not recognized</code> or finds no compiler: start the editor from a <strong>Developer PowerShell for VS 2022</strong>.</li>
		<li>The executables are not in <code>Release\</code> or <code>Debug\</code>: that is expected, they are written in the root of the repository.</li>
	</ul>
</Platform>

<Platform os="linux">
	<ul class="mt-3 list-disc space-y-1.5 pl-6 leading-7">
		<li>raylib fails to find X11 or OpenGL: install the development packages listed in <a href="/user/getting-started" class="text-accent hover:underline">Getting started</a>.</li>
		<li><code>Permission denied</code> on a script: run <code>chmod +x build_rtype.sh</code>.</li>
	</ul>
</Platform>

<p class="mt-5 max-w-[100ch] leading-7">On both systems:</p>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>CMake downloads the dependencies on the first configure: it needs an internet connection.</li>
	<li>The warning about clang-tidy not being found is harmless; configure with <code>-DENABLE_CLANG_TIDY=OFF</code> to hide it.</li>
	<li>If a build still fails after pulling new changes, delete the <code>build</code> folder and build again.</li>
</ul>
