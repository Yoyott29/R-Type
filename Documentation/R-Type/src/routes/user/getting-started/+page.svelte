<svelte:head>
	<title>Getting started - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Getting started</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Build and run the R-Type project</p>

<p class="max-w-[100ch] leading-7">
	R-Type is a reimplementation of the classic shoot 'em up, built around an ECS engine and a
	client / server model. This page covers everything you need to get it running.
</p>

<h2 class="mt-9 text-lg font-semibold">Prerequisites</h2>
<ul class="mt-3 list-disc space-y-1 pl-6 text-[15px]">
	<li>CMake 3.28 or newer</li>
	<li>A C++17 compiler, such as GCC, Clang, or MSVC</li>
	<li>Git</li>
	<li>Linux: OpenGL and X11 development packages</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Build</h2>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>git clone https://github.com/EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4
cd G-ING-500-LIL-5-1-rtype-4
cmake -B build
cmake --build build</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	Run these commands from the repository root. CMake downloads raylib automatically, and the
	executables are generated at the repository root.
</p>

<h2 class="mt-9 text-lg font-semibold">Run</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	On Linux, start the server first, then one client per player in separate terminals:
</p>
<pre
class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./r-type_server
./r-type_client</code></pre>

<p class="mt-6 max-w-[100ch] leading-7">On Windows, use separate PowerShell windows:</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>.\r-type_server.exe
.\r-type_client.exe</code></pre>

<h2 class="mt-9 text-lg font-semibold">First launch checklist</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Run the server before launching a client.</li>
	<li>Run from the repository root so relative <code>Assets/</code> paths resolve.</li>
	<li>Launch up to four clients; player slots are assigned automatically.</li>
	<li>Read <a href="/user/controls" class="text-[#ff8a3d] hover:underline">Controls</a> before playing.</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">Server port</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The server listens on UDP port <code>4242</code> by default. It accepts an optional port:
</p>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./r-type_server 5000</code></pre>