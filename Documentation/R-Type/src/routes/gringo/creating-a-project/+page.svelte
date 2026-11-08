<svelte:head>
	<title>Creating a Project - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Creating a Project</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Start a new game from a template</p>

<p class="max-w-[100ch] leading-7">
	A Gringo game is made of <strong>entities</strong> (the things in the game: a player, a camera, a
	score text...), each with <strong>components</strong> (its data: a position, a sprite, a
	collider...). The engine's <strong>plugins</strong> bring the components and make them work. A
	project holds all of it.
</p>

<h2 class="mt-9 text-lg font-semibold">Create it</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Open the editor (see <a href="/gringo/getting-started" class="text-accent hover:underline">Getting Started</a>) and click <strong>New Game</strong>.</li>
	<li>Choose a template (see below).</li>
	<li>
		Type the project name. Below it, click the renderer to change it if there are several. Press
		<code>Enter</code> to create the project.
	</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">The templates</h2>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"
			><tr><th class="px-4 py-3">Template</th><th class="px-4 py-3">Active plugins</th><th class="px-4 py-3">Starts with</th></tr></thead
		>
		<tbody>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">2D</td><td class="px-4 py-3">2D, Physics2D</td><td class="px-4 py-3">A <code>Player</code> entity with a sprite</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">3D</td><td class="px-4 py-3">3D, Physics3D, InputHandler, Scripts</td><td class="px-4 py-3">A <code>Cube</code>, and a <code>Camera</code> that follows it</td></tr
			>
		</tbody>
	</table>
</div>
<p class="mt-3 max-w-[100ch] leading-7">The template is only a starting point: plugins can be switched on and off later.</p>

<h2 class="mt-9 text-lg font-semibold">Where it is saved</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The project is saved in <code>Gringo Engine/&lt;Name&gt;/</code>:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>&lt;Name&gt;.gringo</code>: the project file (entities, prefabs, plugins, settings).</li>
	<li><code>scripts/</code>: your gameplay code.</li>
	<li><code>assets/</code>: images, sounds, fonts and 3D models.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">To open it again later, click <strong>Load Game</strong> in the main menu and pick it.</p>

<p class="mt-9 max-w-[100ch] leading-7">
	Next: <a href="/gringo/editor-window" class="text-accent hover:underline">The Editor Window</a>.
</p>
