<script>
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';

	const winBuild = `cmake -S "Gringo Engine" -B "Gringo Engine\\build" -A x64
cmake --build "Gringo Engine\\build" --config Release --target Gringo --parallel`;
	const winTests = `cmake -S "Gringo Engine" -B build-unit -A x64 -DGRINGO_BUILD_TESTS=ON
cmake --build build-unit --config Release --target engine_tests network_tests --parallel
ctest --test-dir build-unit -C Release --output-on-failure`;
	const linuxBuild = `./Gringo\\ Engine/build_engine.sh

# the same, by hand
cmake -S "Gringo Engine" -B "Gringo Engine/build"
cmake --build "Gringo Engine/build" --target Gringo -j`;
	const linuxTests = `cmake -S "Gringo Engine" -B build-unit -DGRINGO_BUILD_TESTS=ON
cmake --build build-unit --target engine_tests network_tests -j
ctest --test-dir build-unit --output-on-failure`;
</script>

<svelte:head>
	<title>Building the Engine - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Building the Engine</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Gringo Engine/CMakeLists.txt, a standalone CMake project</p>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>Gringo Engine/</code> is its own CMake project: it builds without the rest of the repository.
</p>

<Platform os="windows">
	<p class="mt-3 leading-7">
		From a <strong>Developer PowerShell for VS 2022</strong>, at the root of the repository.
		<code>build_engine.sh</code> is a Bash script: call CMake directly. Visual Studio is a
		multi-config generator, so the configuration is chosen at build time with
		<code>--config Release</code>.
	</p>
	<pre class={pre}><code>{winBuild}</code></pre>
</Platform>

<Platform os="linux">
	<p class="mt-3 leading-7">From the root of the repository:</p>
	<pre class={pre}><code>{linuxBuild}</code></pre>
</Platform>

<h2 class="mt-9 text-lg font-semibold">Dependencies</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Downloaded by <a href="/developer/cpm" class="text-accent hover:underline">CPM</a> into <code>Gringo Engine/.cpm-cache</code>: raylib 6.0, Asio 1.30.2, miniaudio 0.11.22 and nlohmann/json 3.11.3. Only the renderers link raylib; Asio and miniaudio are given to the plugins.
</p>

<h2 class="mt-9 text-lg font-semibold">The targets</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Target</th><th class="px-4 py-3">Sources</th><th class="px-4 py-3">Adding a file</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>gringo_engine</code></td><td class="px-4 py-3 align-top">An explicit list of <code>src/ECS</code> files</td><td class="px-4 py-3 align-top">Add the new <code>.cpp</code> to the list in <code>add_library(gringo_engine ...)</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>plugin_&lt;Name&gt;</code></td><td class="px-4 py-3 align-top">Every <code>.cpp</code> of <code>src/plugins/&lt;Name&gt;/</code></td><td class="px-4 py-3 align-top">Nothing to do: a new folder is a new plugin, a new file is picked up</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>renderer_&lt;Name&gt;</code></td><td class="px-4 py-3 align-top">Every <code>.cpp</code> of <code>src/renderers/&lt;Name&gt;/</code></td><td class="px-4 py-3 align-top">Nothing to do</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Gringo</code></td><td class="px-4 py-3 align-top">Every <code>.cpp</code> of <code>src/editor/</code></td><td class="px-4 py-3 align-top">Nothing to do, but a new folder needs its include directory</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>gringo_add_plugins</code> does the folder scan: each folder becomes a shared library named after it, and every folder holding a <code>.hpp</code> becomes an include directory. A folder without any <code>.cpp</code> is skipped with a warning.
</p>

<h2 class="mt-9 text-lg font-semibold">Options</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Option</th><th class="px-4 py-3">Default</th><th class="px-4 py-3">Effect</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>ENABLE_CLANG_TIDY</code></td><td class="px-4 py-3 align-top">ON</td><td class="px-4 py-3 align-top">Runs clang-tidy on every file while compiling (if clang-tidy is installed)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>GRINGO_BUILD_TESTS</code></td><td class="px-4 py-3 align-top">OFF</td><td class="px-4 py-3 align-top">Adds the <code>engine_tests</code> and <code>network_tests</code> executables and their CTest tests</td></tr>
		</tbody>
	</table>
</div>

<Platform os="windows">
	<p class="mt-3 leading-7">The CI runs exactly these on <code>windows-latest</code>:</p>
	<pre class={pre}><code>{winTests}</code></pre>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{linuxTests}</code></pre>
</Platform>

<h2 class="mt-9 text-lg font-semibold">Building a game</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The editor builds games with this same CMake project, on Linux and on Windows (where it also builds with <code>--config Release</code> and copies <code>gringo_engine.dll</code> next to the game, since Windows has no rpath): it passes <code>GRINGO_GAME_TARGET</code>, <code>GRINGO_GAME_SOURCE</code> (the generated main), <code>GRINGO_GAME_SCRIPT_DIR</code>, <code>GRINGO_GAME_PLUGINS</code>, <code>GRINGO_GAME_RENDERER</code> and <code>GRINGO_GAME_OUTPUT_DIR</code>, and the project adds one more executable. The plugins listed get their include directories and a <code>GRINGO_USE_&lt;PLUGIN&gt;</code> define, so a script can test <code>#ifdef GRINGO_USE_NETWORK</code>.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	The root <code>CMakeLists.txt</code> of the repository builds R-Type with its own copy of these rules; it is described in <a href="/developer/cmakelists" class="text-accent hover:underline">How CMakeLists works</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Output folders</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Everything is written into <code>Gringo Engine/</code> itself (<code>plugins/</code>, <code>renderers/</code>, the library and the editor), not into the build folder: that is where the <code>PluginLoader</code> looks. The output paths end with <code>$&lt;0:&gt;</code>, an empty generator expression that stops Visual Studio from adding a <code>Release/</code> or <code>Debug/</code> subfolder.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/gringo/developer/ecs" class="text-accent hover:underline">How the ECS works</a>.
</p>
