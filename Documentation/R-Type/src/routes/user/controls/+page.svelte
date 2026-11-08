<svelte:head>
	<title>Controls - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Controls</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Keyboard, mouse and player actions</p>

<p class="mt-3 max-w-[100ch] leading-7">
	The client reads the keyboard and sends it to the authoritative server, which moves your ship.
</p>

<h2 class="mt-9 text-lg font-semibold">In the match</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Key</th><th class="px-4 py-3">Action</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Up</code> / <code>Z</code></td><td class="px-4 py-3 align-top">Move up</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Down</code> / <code>S</code></td><td class="px-4 py-3 align-top">Move down</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Left</code> / <code>Q</code></td><td class="px-4 py-3 align-top">Move left</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Right</code> / <code>D</code></td><td class="px-4 py-3 align-top">Move right</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">(none)</td><td class="px-4 py-3 align-top">Fire: automatic, a shot every 0.13 s</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Space</code> (hold, then release)</td><td class="px-4 py-3 align-top">Charged shot: the longer you hold, the bigger the beam</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Esc</code> or close the window</td><td class="px-4 py-3 align-top">Quit the game</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">In the lobby</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Input</th><th class="px-4 py-3">Action</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Left click on <code>START</code></td><td class="px-4 py-3 align-top">Start the match (only the host's click counts)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Left click on <code>SETTINGS</code></td><td class="px-4 py-3 align-top">Open the settings panel</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Settings</h2>
<p class="mt-3 max-w-[100ch] leading-7">The settings panel opens from the lobby. Everything is done with a left click:</p>
<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Button</th><th class="px-4 py-3">Action</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>UP</code>, <code>DOWN</code>, <code>LEFT</code>, <code>RIGHT</code>, <code>SHOOT</code></td><td class="px-4 py-3 align-top">Change the key of that action: click, then press the new key (<code>SHOOT</code> is the charged shot). <code>Esc</code> cancels</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>COLORS</code></td><td class="px-4 py-3 align-top">Switch the color filter: normal, protanopia, deuteranopia, tritanopia</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>BACK</code></td><td class="px-4 py-3 align-top">Close the panel and go back to the lobby</td></tr>
		</tbody>
	</table>
</div>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Each button shows the keys of its action. A new key replaces them all: after changing <code>UP</code>, neither the up arrow nor the default letter moves the ship up anymore.</li>
	<li>The keys you choose are kept until the client is closed.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Keyboard layout</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Keys are found by their <strong>position</strong>, not by the letter printed on them. They are named after an AZERTY keyboard:
</p>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>On an AZERTY keyboard, the keys labelled Z, Q, S and D move the ship.</li>
	<li>On a QWERTY keyboard, the keys labelled W, A, S and D do.</li>
	<li>The arrow keys and Space work on every layout.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Input behavior</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The keys are sent to the server as bits, and the server applies them on its game tick (60 per second).</li>
	<li>Pressing two opposite keys at once cancels the movement.</li>
	<li>While you hold Space, the automatic fire stops; releasing it fires the charged shot.</li>
	<li>The ship stays inside the 1920 by 1080 play area.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	See <a href="/user/accessibility" class="text-accent hover:underline">Accessibility</a> for what the controls mean for players with motor disabilities.
</p>
