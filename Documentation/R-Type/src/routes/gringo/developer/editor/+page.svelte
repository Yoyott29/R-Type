<script>
	const code0 = `{
    "projectName": "MyGame",
    "renderer": "RaylibRenderer",
    "plugins": ["2D", "Physics2D", "Scripts"],
    "pluginSettings": { "Physics2D": { "GravityY": "980" } },
    "entities": [
        { "id": 0, "name": "Player", "components": [
            { "name": "Transform", "properties": [
                { "id": 0, "name": "PositionX", "type": 1, "value": "960" } ] } ] } ],
    "prefabs": [ ... ]
}`;
</script>

<svelte:head>
	<title>How the editor works - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">How the editor works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">src/editor: menus, the .gringo format and the project builder</p>

<h2 class="mt-9 text-lg font-semibold">The parts</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Folder</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>main.cpp</code></td><td class="px-4 py-3 align-top">Opens the window and switches between menus; <code>--build</code> skips the window</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>menu/</code></td><td class="px-4 py-3 align-top">One class per screen, all deriving from <code>AMenu</code> (buttons, texts, rectangles): main, template, name, load, and the editor itself (<code>EditorMenu</code>, split into objects, components, properties and animations)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>renderer/</code></td><td class="px-4 py-3 align-top">The editor's own small renderer interface (<code>IEditorRenderer</code>), with a raylib implementation</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>parser/</code></td><td class="px-4 py-3 align-top"><code>Parser</code>: reads and writes <code>.gringo</code> files, lists the projects, reads script parameters</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>catalog/</code></td><td class="px-4 py-3 align-top"><code>ComponentCatalog</code>: every component and plugin setting the editor can offer</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>builder/</code></td><td class="px-4 py-3 align-top"><code>ProjectBuilder</code>: checks a project, writes its main(s) and compiles them; <code>CommandLineBuild</code> for <code>--build</code></td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">The .gringo file</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A JSON file written by the <code>Parser</code>:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	Every value is saved as text; <code>type</code> is the <code>valueType</code> number. An entity's <code>id</code> is what <code>ChildOf.Parent</code> and scripts refer to; in a game, the <code>EditorEntityIds</code> resource maps it to the real entity.
</p>

<h2 class="mt-9 text-lg font-semibold">The component catalog</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	When the editor starts, the <code>ComponentCatalog</code> takes the components of the application (Transform, ChildOf...), then loads every plugin library and calls only <code>registerComponents</code> and <code>settings</code>. Every component with a <code>ComponentDescription</code> becomes a template the editor can add, tagged with its plugin. A plugin that fails to load is skipped with a message.
</p>

<h2 class="mt-9 text-lg font-semibold">Building a project</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>ProjectBuilder::build</code>, from the Build button or <code>--build</code>:
</p>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li><strong>Checks</strong>: every file property (scripts, assets) points to an existing file, <code>ChildOf</code> parents exist and make no loop, every <code>Prefab</code> component names a prefab of the project, the renderer exists, and each component's plugin is active. Any problem stops the build with a message.</li>
	<li><strong>Variants</strong>: one executable named after the project, or one per build variant of the active plugins (Network: <code>&lt;name&gt;_server</code>, headless, and <code>&lt;name&gt;_client</code>). A headless variant leaves out the plugins that need a renderer and their components.</li>
	<li><strong>Generated main</strong>: for each variant, <code>main.cpp</code> or <code>&lt;variant&gt;_main.cpp</code> in the project folder. It includes every script used, adds the plugins, and creates each Script component by its file name (see <a href="/gringo/developer/scripts" class="text-accent hover:underline">Scripts</a>).</li>
	<li><strong>Compile</strong>: runs CMake on <code>Gringo Engine/</code> with the <code>GRINGO_GAME_*</code> variables (see <a href="/gringo/developer/building" class="text-accent hover:underline">Building the Engine</a>), in <code>&lt;project&gt;/build</code>, and writes the executable in the project folder.</li>
</ol>

<p class="mt-3 max-w-[100ch] leading-7">
	The generated mains are ordinary C++: they can be read to see exactly how a game is put together, but they are written again at each build, so never edit them.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/gringo/developer/scripts" class="text-accent hover:underline">Scripts</a>.
</p>
