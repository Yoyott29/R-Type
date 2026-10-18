<svelte:head><title>Troubleshooting - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Troubleshooting</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Common launch and gameplay problems</p>
<h2 class="mt-9 text-lg font-semibold">Address already in use</h2>
<p class="mt-3 max-w-[100ch] leading-7">Another server is listening on UDP port 4242. Stop it or start the server on another port, then configure the client to match.</p>
<h2 class="mt-9 text-lg font-semibold">Client crashes after connecting</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Rebuild both executables after changing the snapshot protocol.</li>
	<li>Run compatible server and client versions.</li>
	<li>Check that both sides use the same protocol version.</li>
	<li>Run the client from the repository root so assets resolve.</li>
</ul>
<h2 class="mt-9 text-lg font-semibold">Build failures</h2>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>cmake -B build
cmake --build build --target r-type_client r-type_server -j2</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">If dependencies are missing, let CMake download raylib and Asio, then retry. Use <code>clang-format --dry-run --Werror</code> to check formatting.</p>
