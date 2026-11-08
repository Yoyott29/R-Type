<svelte:head>
	<title>Plugins and Settings - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Plugins and Settings</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Choose what the engine brings to your game</p>

<p class="max-w-[100ch] leading-7">
	Each feature of the engine is a plugin. A game only loads the plugins it uses, and a component can
	only be added while its plugin is on.
</p>

<h2 class="mt-9 text-lg font-semibold">Plugins</h2>
<p class="mt-3 max-w-[100ch] leading-7">Click <strong>Plugins</strong> in the top bar and switch each plugin <code>ON</code> or <code>OFF</code>.</p>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"
			><tr><th class="px-4 py-3">Plugin</th><th class="px-4 py-3">What it brings</th></tr></thead
		>
		<tbody>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">2D</td><td class="px-4 py-3">Sprites, animations, 2D particles</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">3D</td><td class="px-4 py-3">3D models, cameras, 3D particles</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Physics2D / Physics3D</td><td class="px-4 py-3">Velocity, gravity, colliders</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">InputHandler</td><td class="px-4 py-3">The keyboard</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Scripts</td><td class="px-4 py-3">Your own gameplay code</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Audio</td><td class="px-4 py-3">Music and sounds</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">UI</td><td class="px-4 py-3">Texts and buttons</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Network</td><td class="px-4 py-3">Makes the game multiplayer: it is built as a server and a client</td></tr
			>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Settings</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Click <strong>Settings</strong> to see the project's renderer (click <code>USE</code> to pick
	another one) and the settings of the active plugins, for example:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Physics2D: <code>GravityX</code> and <code>GravityY</code>.</li>
	<li>
		Network: <code>Host</code> and <code>Port</code> (the default address, <code>127.0.0.1:4242</code>)
		and <code>PlayerPrefabs</code>, the prefab given to each player who joins.
	</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">Click a setting to change it, the same way as a component value (see <a href="/gringo/entities-and-components" class="text-accent hover:underline">Entities and Components</a>).</p>

<p class="mt-9 max-w-[100ch] leading-7">
	Next: <a href="/gringo/entities-and-components" class="text-accent hover:underline">Entities and Components</a>.
</p>
