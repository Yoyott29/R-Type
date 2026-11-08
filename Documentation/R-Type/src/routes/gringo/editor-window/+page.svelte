<svelte:head>
	<title>The Editor Window - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">The Editor Window</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">What is where in the editor</p>

<p class="max-w-[100ch] leading-7">
	Everything in the editor is done with the mouse (left click), and the keyboard to type names and
	values.
</p>

<h2 class="mt-9 text-lg font-semibold">Layout</h2>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"
			><tr><th class="px-4 py-3">Where</th><th class="px-4 py-3">What</th></tr></thead
		>
		<tbody>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Left column</td><td class="px-4 py-3"><strong>Entities</strong>: the objects in the game when it starts. A child entity is shown under its parent, one step to the right.</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Right column</td><td class="px-4 py-3"><strong>Prefabs</strong>: models of entities the game creates while it runs, like an enemy or a bullet.</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Middle</td><td class="px-4 py-3">The components of the selected entity or prefab, or the open panel.</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Top bar</td><td class="px-4 py-3">The <code>Settings</code>, <code>Plugins</code> and <code>Animation</code> panels, then <code>Build</code> and <code>Back</code>.</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Bottom bar</td><td class="px-4 py-3">The project name and <code>Save Project</code>.</td></tr
			>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">The lists</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Each column shows 15 items per page: use <code>&lt;</code> and <code>&gt;</code> under it to change page.</li>
	<li><code>Add Entity</code> and <code>Add Prefab</code> are at the end of the lists.</li>
	<li>Click an item to select it: its components appear in the middle.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Saving</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Click <strong>Save Project</strong> from time to time: a "File saved..." message confirms it.</li>
	<li><code>Back</code> saves too, before going back to the menu.</li>
	<li><code>Build</code> saves before building.</li>
</ul>

<p class="mt-9 max-w-[100ch] leading-7">
	Next: <a href="/gringo/plugins-and-settings" class="text-accent hover:underline">Plugins and Settings</a>.
</p>
