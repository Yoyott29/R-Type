<svelte:head>
	<title>Controls - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Controls</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Keyboard, mouse and player actions</p>

<p class="max-w-[100ch] leading-7">
	The client reads the keyboard and the mouse and sends them to the server, which moves your rat.
</p>

<h2 class="mt-9 text-lg font-semibold">Keys and mouse</h2>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"
			><tr><th class="px-4 py-3">Key</th><th class="px-4 py-3">Action</th></tr></thead
		>
		<tbody>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">Mouse</td><td class="px-4 py-3"
					>Turn the camera around your rat, and look up or down</td
				></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">Z</td><td class="px-4 py-3"
					>Run the way the camera looks</td
				></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">S</td><td class="px-4 py-3">Run the other way</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">Space</td><td class="px-4 py-3"
					>Jump (only when your rat is on the floor)</td
				></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">E</td><td class="px-4 py-3">Throw a mouse trap</td></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">Left click</td><td class="px-4 py-3"
					>Click <code>START</code> in the lobby (host only)</td
				></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">Esc or close the window</td><td class="px-4 py-3"
					>Quit the game</td
				></tr
			>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Keyboard layout</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Keys are found by their <strong>position</strong>, not by the letter printed on them. They are
	named after an AZERTY keyboard:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>On an AZERTY keyboard, use the keys labelled Z, S, E and Space.</li>
	<li>On a QWERTY keyboard, <code>Z</code> is the key labelled <code>W</code> (same place); S, E and Space are the same.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Input behavior</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>
		Your rat runs where the camera looks: turn the mouse to steer, the rat turns to face the way it
		runs.
	</li>
	<li>Pressing Z and S together cancels the movement.</li>
	<li>
		Holding Space jumps only once: release it and press again to jump again, once you are back on the
		floor.
	</li>
	<li>Each press of E throws one trap; holding it does not throw more.</li>
	<li>
		During the race the mouse cursor is hidden and held in the window. Nobody can move before the
		countdown says GO, nor after the race ends.
	</li>
	<li>While you are stunned by a spatula, Z, S and Space do nothing.</li>
</ul>

<p class="mt-9 max-w-[100ch] leading-7">
	Next: the rules of the race in
	<a href="/trackntrap/gameplay" class="text-accent hover:underline">Gameplay</a>.
</p>
