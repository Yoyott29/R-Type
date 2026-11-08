<script>
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';
	const code0 = `on:
  pull_request:
    branches: [main]`;
	const winCode = `cmake -S "Gringo Engine" -B build-unit -A x64 -DGRINGO_BUILD_TESTS=ON -DENABLE_CLANG_TIDY=OFF
cmake --build build-unit --config Release --target engine_tests network_tests --parallel
ctest --test-dir build-unit -C Release --output-on-failure`;
	const code1 = `cmake -S "Gringo Engine" -B build-unit -DGRINGO_BUILD_TESTS=ON -DENABLE_CLANG_TIDY=OFF
cmake --build build-unit --target engine_tests network_tests --parallel
ctest --test-dir build-unit --output-on-failure`;
</script>

<svelte:head>
	<title>Tests - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Tests</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">The tests workflow: formatting, Linux tests, engine unit tests and Windows</p>

<h2 class="mt-9 text-lg font-semibold">When it runs</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>.github/workflows/tests.yml</code> runs on every pull request targeting <code>main</code>. A new push to the same pull request cancels the run still in progress.
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<h2 class="mt-9 text-lg font-semibold">Where it runs</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	On GitHub-hosted runners, in the team's repository only: every job has <code>if: github.repository != 'EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4'</code>, so the Epitech mirror does not run it again.
</p>

<h2 class="mt-9 text-lg font-semibold">Jobs</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Job</th><th class="px-4 py-3">Runner</th><th class="px-4 py-3">Steps</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>lint</code></td><td class="px-4 py-3 align-top">ubuntu-latest</td><td class="px-4 py-3 align-top">Installs clang-format 21.1.8 and checks every tracked <code>.cpp</code> / <code>.hpp</code> file with <code>Norms/.clang-format</code>, except the generated <code>*_main.cpp</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>tests</code></td><td class="px-4 py-3 align-top">ubuntu-latest</td><td class="px-4 py-3 align-top">Installs the OpenGL / X11 packages and clang-tidy, restores the CPM cache, builds the editor and regenerates the R-Type mains, runs <code>./tests/tests.sh</code>, builds and runs the engine unit tests with CTest, then builds everything with clang-tidy</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>windows</code></td><td class="px-4 py-3 align-top">windows-latest</td><td class="px-4 py-3 align-top">Builds the client and the server with MSVC (x64, Release), runs the engine unit tests, then starts <code>r-type_server.exe</code> and checks it is still running and listening on UDP 4242 after 5 seconds</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	The Windows job only builds the client: the runner has no GPU to open its window.
</p>

<h2 class="mt-9 text-lg font-semibold">The engine unit tests</h2>

<Platform os="windows">
	<p class="mt-3 leading-7">From a <strong>Developer PowerShell for VS 2022</strong>, as the <code>windows</code> job does:</p>
	<pre class={pre}><code>{winCode}</code></pre>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{code1}</code></pre>
</Platform>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>engine_tests</code> covers the scheduler, the thread pool, <code>parallelFor</code> and the destroy queue; <code>network_tests</code> the packet parsing, messages, transport, sockets, replication and players. Run the same commands locally before a pull request that changes the engine.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The CPM cache is keyed on <code>CMakeLists.txt</code> and <code>CPM.cmake</code>: dependencies are downloaded again only when they change.</li>
	<li>To check the formatting locally, run the same command as the lint job (see <a href="/developer/clang-format" class="text-accent hover:underline">clang-format</a>).</li>
	<li>A failing job is shown on the pull request: open its log in the Checks tab.</li>
</ul>
