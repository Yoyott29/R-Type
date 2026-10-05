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
	<li>A C++ compiler (GCC or Clang)</li>
	<li>CMake 3.20 or newer</li>
	<li>Git</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Build</h2>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>https://github.com/EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4
cd rtype
cmake -B build
cmake --build build</code></pre>

<h2 class="mt-9 text-lg font-semibold">Run</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Start the server first, then one or more clients:
</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./build/r-type_server
./build/r-type_client</code></pre>