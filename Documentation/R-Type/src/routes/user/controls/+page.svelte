<svelte:head><title>Controls - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Controls</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Keyboard input and player actions</p>
<p class="max-w-[100ch] leading-7">The client reads the keyboard and sends input to the authoritative server.</p>
<h2 class="mt-9 text-lg font-semibold">Keyboard layout</h2>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Key</th><th class="px-4 py-3">Action</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">W / Up</td><td class="px-4 py-3">Move up</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">S / Down</td><td class="px-4 py-3">Move down</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">A / Left</td><td class="px-4 py-3">Move left</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">D / Right</td><td class="px-4 py-3">Move right</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Space</td><td class="px-4 py-3">Fire</td></tr>
		</tbody>
	</table>
</div>
<h2 class="mt-9 text-lg font-semibold">Input behavior</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Inputs are encoded as a bit mask and sent over UDP.</li>
	<li>The server applies input during its fixed game tick.</li>
	<li>Firing is limited by a server-side cooldown.</li>
	<li>The player remains inside the 1920 by 1080 play area.</li>
</ul>
