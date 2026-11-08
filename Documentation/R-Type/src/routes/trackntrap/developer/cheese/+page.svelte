<svelte:head>
	<title>Cheese - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Cheese</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">CheeseSpawner puts and animates the cheese, Cheeses tracks what was eaten</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">CheeseSpawner</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/CheeseSpawner.hpp</code> &middot; on <strong>Cheeses</strong> &middot; runs on <strong>the server</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>prefab</code></td><td class="px-4 py-3 align-top">Cheese</td><td class="px-4 py-3 align-top">What is put on the track</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>count</code></td><td class="px-4 py-3 align-top">4</td><td class="px-4 py-3 align-top">Pieces on the track at once</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>respawn</code></td><td class="px-4 py-3 align-top">5</td><td class="px-4 py-3 align-top">Seconds before an eaten piece is replaced</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>height</code></td><td class="px-4 py-3 align-top">2</td><td class="px-4 py-3 align-top">Height above the floor</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>bobHeight</code></td><td class="px-4 py-3 align-top">0.5</td><td class="px-4 py-3 align-top">How far it floats up and down</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>bobSpeed</code></td><td class="px-4 py-3 align-top">0.5</td><td class="px-4 py-3 align-top">Up and down moves per second</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>spinSpeed</code></td><td class="px-4 py-3 align-top">90</td><td class="px-4 py-3 align-top">Degrees per second around itself</td></tr>
		</tbody>
	</table>
</div>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>SPOTS</code> lists 38 places along the lanes. A new piece goes on a free spot, at a random distance (up to 8) across the lane and a random rotation.</li>
	<li>Every tick it floats and spins every piece (each with its own phase, so they do not move together); the Transform is replicated, so the clients see it.</li>
	<li>It destroys the pieces eaten since the last tick and, <code>respawn</code> seconds later, puts a new one on another free spot.</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Cheeses</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>Cheeses.hpp</code>, a resource of the server, between the spawner and the rats:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Function</th><th class="px-4 py-3">Called by</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>add</code></td><td class="px-4 py-3 align-top">CheeseSpawner</td><td class="px-4 py-3 align-top">A new piece is on the track</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>eat</code></td><td class="px-4 py-3 align-top">RatPlayerController</td><td class="px-4 py-3 align-top">True if the piece was still there: it is now eaten, so two rats cannot eat it</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>takeEaten</code></td><td class="px-4 py-3 align-top">CheeseSpawner</td><td class="px-4 py-3 align-top">The pieces eaten since the last call</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	The boost itself (<code>boost_speed</code>, <code>boost_time</code>) is a parameter of the rat, see <a href="/trackntrap/developer/rat" class="text-accent hover:underline">The Rat</a>.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/trackntrap/developer/how-to-modify" class="text-accent hover:underline">How to Modify</a>.
</p>
