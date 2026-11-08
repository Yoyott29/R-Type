<script>
	const code0 = `  game project      .gringo scene, scripts/, assets/ ──► generated client_main / server_main
       │
  editor            Gringo: menus, Parser, ComponentCatalog, ProjectBuilder
       │
  runtime           ProjectComponentManager: loads a .gringo into the scene
       │
  plugins           2D  3D  Physics2D  Physics3D  InputHandler  Scripts  Audio  UI  Network
       │            (one shared library each, loaded when the game starts)
  renderers         RaylibRenderer (one shared library each, the only code calling raylib)
       │
  core              libgringo_engine: Application, Scene, ECS, schedules, resources, events`;
</script>

<svelte:head>
	<title>Architecture - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Architecture</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">The parts of the engine and how a game is made from them</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Gringo is split in layers. Each one only knows the layers under it: the core never includes a plugin, a plugin never includes a game, and nothing in <code>Gringo Engine/</code> knows R-Type or TrackNTrap.
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<h2 class="mt-9 text-lg font-semibold">The folders</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Folder</th><th class="px-4 py-3">What it holds</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/ECS/app</code></td><td class="px-4 py-3 align-top"><code>Application</code> (the game loop, plugin and renderer loading), <code>PluginLoader</code>, <code>IPlugin</code>, <code>BuildVariant</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/ECS/core</code></td><td class="px-4 py-3 align-top"><code>Scene</code> and its managers (entities, components, systems, resources), <code>Events</code> (the event queues), the thread pool and <code>parallelFor</code>, the fixed tick clock</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/ECS/systems</code></td><td class="px-4 py-3 align-top"><code>System</code>, the <code>SystemManager</code>, the schedule graph and runner (parallel systems), <code>SystemAccess</code>, the hierarchy and prefab systems</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/ECS/components</code></td><td class="px-4 py-3 align-top">Components every game has: <code>Transform</code>, <code>ChildOf</code>, <code>ColorRGBA</code>, <code>Shape</code>, <code>PrefabInstance</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/ECS/resources</code></td><td class="px-4 py-3 align-top">Resources the application adds: <code>AppAlive</code>, <code>CommandLine</code>, <code>ProjectSettings</code>, <code>GameClock</code>, <code>ProjectAssets</code>, <code>BuildVariantName</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/ECS/prefabs</code>, <code>input</code>, <code>errors</code></td><td class="px-4 py-3 align-top">Prefabs and the <code>PrefabsManager</code>, the key list and <code>ActionInput</code>, the engine errors</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/plugins/&lt;Name&gt;</code></td><td class="px-4 py-3 align-top">One plugin per folder, see <a href="/gringo/developer/plugins" class="text-accent hover:underline">How plugins work</a></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/renderers/&lt;Name&gt;</code></td><td class="px-4 py-3 align-top">One renderer per folder, see <a href="/gringo/developer/renderers" class="text-accent hover:underline">Renderers</a></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/runtime</code></td><td class="px-4 py-3 align-top"><code>ProjectComponentManager</code>, compiled into every game to load its <code>.gringo</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>src/editor</code></td><td class="px-4 py-3 align-top">The editor, see <a href="/gringo/developer/editor" class="text-accent hover:underline">How the editor works</a></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>tests/</code></td><td class="px-4 py-3 align-top">The engine and network unit tests, see <a href="/gringo/developer/building" class="text-accent hover:underline">Building the Engine</a></td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">What the build makes</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Output</th><th class="px-4 py-3">In <code>Gringo Engine/</code></th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">The engine library</td><td class="px-4 py-3 align-top"><code>libgringo_engine.so</code> (<code>gringo_engine.dll</code> on Windows)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">The plugins</td><td class="px-4 py-3 align-top"><code>plugins/2D.so</code>, <code>plugins/Network.so</code>...</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">The renderers</td><td class="px-4 py-3 align-top"><code>renderers/RaylibRenderer.so</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">The editor</td><td class="px-4 py-3 align-top"><code>Gringo</code></td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	A game is an executable linked with the engine library only. Its plugins and its renderer are opened when it starts, by name, so a game carries only what it uses.
</p>

<h2 class="mt-9 text-lg font-semibold">How a game starts</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The editor generates the game's <code>main</code> (see <a href="/gringo/developer/editor" class="text-accent hover:underline">How the editor works</a>). It always does the same steps:
</p>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Reads the <code>.gringo</code> file found next to the executable.</li>
	<li>Creates the <code>Application</code>, which registers <code>Transform</code>, <code>ChildOf</code> and <code>PrefabInstance</code> and adds the base resources.</li>
	<li>Adds the <code>ProjectSettings</code> (the plugin settings of the project) and the <code>BuildVariantName</code> (<code>"client"</code>, <code>"server"</code>, or empty).</li>
	<li>Opens the renderer and its window, unless the executable is headless.</li>
	<li>Adds each active plugin: <code>registerComponents</code> then <code>build</code>.</li>
	<li>Creates the entities and prefabs of the <code>.gringo</code> with the <code>ProjectComponentManager</code>, creating each script by its name.</li>
	<li>Runs the loop, see <a href="/gringo/developer/game-loop" class="text-accent hover:underline">How the game loop works</a>.</li>
</ol>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/gringo/developer/building" class="text-accent hover:underline">Building the Engine</a>.
</p>
