<script>
	import LinuxOnly from '$lib/components/LinuxOnly.svelte';
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';
	const winCode = `cmake -S . -B build -A x64
cmake --build build --config Release --target r-type_client r-type_server -j`;
	const code0 = `./build_rtype.sh

# the same, by hand
cmake -S . -B build
cmake --build build --target r-type_client r-type_server`;
	const code1 = `rtype_add_game(r-type_client \${GRINGO_REPOSITORY_DIR}/R-Type/client_main.cpp RaylibRenderer
               Physics2D 2D Scripts InputHandler Network Audio UI)
rtype_add_game(r-type_server \${GRINGO_REPOSITORY_DIR}/R-Type/server_main.cpp ""
               Physics2D Scripts Network UI)`;
</script>

<svelte:head>
	<title>How the CMakeLists works - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">How the CMakeLists works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">A walk through the root CMakeLists.txt, which builds r-type_client and r-type_server</p>

<p class="mt-3 max-w-[100ch] leading-7">
	The root <code>CMakeLists.txt</code> builds the two R-Type programs and everything they load. The engine has its own standalone CMake project in <code>Gringo Engine/</code> (see <a href="/gringo/developer/building" class="text-accent hover:underline">Building the Engine</a>); the root one repeats its engine and plugin rules so R-Type builds in one step.
</p>

<Platform os="windows">
	<p class="mt-3 leading-7">
		From a <strong>Developer PowerShell for VS 2022</strong>. Visual Studio is a multi-config generator: the configuration is chosen when building, with <code>--config Release</code>.
	</p>
	<pre class={pre}><code>{winCode}</code></pre>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{code0}</code></pre>
	<LinuxOnly />
</Platform>

<h2 class="mt-9 text-lg font-semibold">1. Project settings</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>CMake 3.28 or newer, C++20, a Release build by default.</li>
	<li>The executables are written in the root of the repository; the engine library, plugins and renderers in <code>Gringo Engine/</code>. Every output path ends with <code>$&lt;0:&gt;</code>, an empty generator expression that stops Visual Studio from adding a <code>Release/</code> folder.</li>
	<li>On Windows, the engine DLL goes next to the executables, since Windows has no rpath.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">2. Dependencies</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Downloaded by <a href="/developer/cpm" class="text-accent hover:underline">CPM</a> into <code>.cpm-cache</code>: raylib 6.0, Asio 1.30.2 (header only, wrapped in an <code>asio</code> interface target), miniaudio 0.11.22 (compiled from its <code>miniaudio.c</code>) and nlohmann/json 3.11.3.
</p>

<h2 class="mt-9 text-lg font-semibold">3. The engine and its plugins</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>gringo_engine</code>: a shared library made of the files of <code>Gringo Engine/src/ECS</code>, listed one by one. A new engine <code>.cpp</code> must be added here <strong>and</strong> in <code>Gringo Engine/CMakeLists.txt</code>.</li>
	<li><code>gringo_add_plugins</code>: one shared library per folder of <code>Gringo Engine/src/plugins</code> and of <code>src/renderers</code>, with all its <code>.cpp</code> files. Nothing to add for a new plugin.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">4. The two programs</h2>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code1}</code></pre>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>rtype_add_game</code> compiles the generated main and the engine's <code>ProjectComponentManager</code>, with <code>R-Type/scripts</code> as include directory.</li>
	<li>The plugin list gives the program the plugins' headers and a <code>GRINGO_USE_&lt;NAME&gt;</code> define (<code>PlayerScript</code> uses <code>#ifdef GRINGO_USE_NETWORK</code>). The plugins are still loaded at runtime; the list must match the plugins of <code>R-Type.gringo</code>.</li>
	<li>If a generated main is missing, configuration stops and says how to generate it.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	Scripts are header-only and included by the generated mains: adding a script needs no CMake change, only a new <code>--build</code> of the project.
</p>

<h2 class="mt-9 text-lg font-semibold">5. Warnings and clang-tidy</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>-Wall -Wextra</code> (or <code>/W4</code> with MSVC) on every target of the project.</li>
	<li><code>ENABLE_CLANG_TIDY</code> (on by default) runs <a href="/developer/clang-tidy" class="text-accent hover:underline">clang-tidy</a> on every file while compiling, if it is installed.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The first configuration downloads the dependencies; later ones use the cache.</li>
	<li>After pulling changes that add or move files, configure again (<code>cmake -S . -B build</code>).</li>
	<li>The engine unit tests are built by the engine's own CMake project, see <a href="/developer/running-tests" class="text-accent hover:underline">Running Tests</a>.</li>
</ul>
