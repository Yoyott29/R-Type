<svelte:head>
	<title>Mouse Traps - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Mouse Traps</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">TrapPlacer throws them, the Traps resource keeps track of them</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">TrapPlacer</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/TrapPlacer.hpp</code> &middot; on <strong>Rat1 to Rat4 (prefabs)</strong> &middot; runs on <strong>the server</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>prefab</code></td><td class="px-4 py-3 align-top">MouseTrap</td><td class="px-4 py-3 align-top">What is thrown</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>count</code></td><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">Traps per race</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>distance</code></td><td class="px-4 py-3 align-top">12</td><td class="px-4 py-3 align-top">How far in front of the rat it appears</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>speed</code></td><td class="px-4 py-3 align-top">150</td><td class="px-4 py-3 align-top">How fast it slides</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>lifetime</code></td><td class="px-4 py-3 align-top">6</td><td class="px-4 py-3 align-top">Seconds before it disappears</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>immunity</code></td><td class="px-4 py-3 align-top">5</td><td class="px-4 py-3 align-top">Seconds during which it cannot catch the rat that threw it</td></tr>
		</tbody>
	</table>
</div>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>On the press of E (not while held), while the rat can move and has traps left: creates the prefab <code>distance</code> in front of the rat, on the floor, turned like the rat, with a Velocity3D of <code>speed</code> forward.</li>
	<li>Remembers where the rat's feet are while it is on the floor, so a trap thrown during a jump still slides on the floor.</li>
	<li>Lowers <code>RatStatus.trapsLeft</code>, which the HUD shows.</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Traps</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>Traps.hpp</code>, a resource of the server, lists the flying traps with their owner, immunity and life left:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Function</th><th class="px-4 py-3">Called by</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>add</code></td><td class="px-4 py-3 align-top">TrapPlacer</td><td class="px-4 py-3 align-top">A trap was thrown</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>covers</code></td><td class="px-4 py-3 align-top">RatPlayerController</td><td class="px-4 py-3 align-top">True if this trap belongs to this rat and is still immune</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>hit</code></td><td class="px-4 py-3 align-top">RatPlayerController</td><td class="px-4 py-3 align-top">The trap caught a rat: it is spent</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>elapse</code></td><td class="px-4 py-3 align-top">RaceDirector, every tick</td><td class="px-4 py-3 align-top">Ages every trap; the expired ones are spent</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>takeDone</code></td><td class="px-4 py-3 align-top">RaceDirector</td><td class="px-4 py-3 align-top">The spent traps, which it destroys</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	A trap is not in <code>Hazards</code>, so a rat it catches goes back to the start.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/trackntrap/developer/obstacles" class="text-accent hover:underline">Obstacles</a>.
</p>
