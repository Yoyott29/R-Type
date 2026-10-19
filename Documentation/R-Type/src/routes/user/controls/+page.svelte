<svelte:head><title>Controls - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Controls</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Keyboard input and player actions</p>
<p class="max-w-[100ch] leading-7">The client reads the keyboard and sends input to the authoritative server.</p>
<h2 class="mt-9 text-lg font-semibold">Keyboard layout</h2>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Key</th><th class="px-4 py-3">Action</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Up arrow</td><td class="px-4 py-3">Move up</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Down arrow</td><td class="px-4 py-3">Move down</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Left arrow</td><td class="px-4 py-3">Move left</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Right arrow</td><td class="px-4 py-3">Move right</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">(none)</td><td class="px-4 py-3">Fire: automatic, a bullet every 0.2 s</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Esc or close the window</td><td class="px-4 py-3">Quit the game</td></tr>
		</tbody>
	</table>
</div>
<h2 class="mt-9 text-lg font-semibold">Letter keys</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The code also binds the keys reported as <code>Z</code> (up), <code>Q</code> (left), <code>S</code> (down) and
	<code>D</code> (right). raylib reads keys by their position on a US QWERTY keyboard, so:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>On a QWERTY keyboard, the keys labelled Z, Q, S and D work, but they are not grouped.</li>
	<li>On an AZERTY keyboard, the keys labelled W, A, S and D work.</li>
	<li>The arrow keys work on every layout: use them if in doubt.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	<code>Space</code> sends a "charge" bit to the server, but the server does not use it yet: it has no effect.
	The keys cannot be changed in the game yet.
</p>
<h2 class="mt-9 text-lg font-semibold">Input behavior</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Inputs are encoded as a bit mask and sent over UDP, once per frame.</li>
	<li>The server applies input during its fixed game tick (60 per second).</li>
	<li>The ship fires on its own, limited by a server-side cooldown of 0.2 s.</li>
	<li>Pressing two opposite keys at once cancels the movement.</li>
	<li>The player remains inside the 1920 by 1080 play area.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	See <a href="/user/accessibility" class="text-[#ff8a3d] hover:underline">Accessibility</a> for what the controls
	mean for players with motor disabilities.
</p>
