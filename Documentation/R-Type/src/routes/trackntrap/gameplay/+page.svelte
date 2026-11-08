<svelte:head>
	<title>Gameplay - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Gameplay</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Laps, obstacles, cheese and traps</p>

<p class="max-w-[100ch] leading-7">
	You are a rat racing around a kitchen track against up to three other players. The first rat to
	finish <strong>3 laps</strong> wins.
</p>

<h2 class="mt-9 text-lg font-semibold">The race</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>
		After the host clicks START, a <strong>3, 2, 1, GO</strong> countdown runs: nobody can move until
		GO.
	</li>
	<li>
		A lap only counts if you go all the way around the track before crossing the finish line:
		shortcuts do not count.
	</li>
	<li>Rats are solid: you bump into the other players instead of running through them.</li>
	<li>
		When you are sent back to the start, the lap you were running starts again; the laps you already
		finished are kept.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">On screen</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The race time.</li>
	<li>Your lap, for example <code>LAP 1/3</code>.</li>
	<li>Your mouse traps and how many are left.</li>
	<li>
		At the end: <code>YOU WIN!</code> if you won, or which rat won, with the winning time.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Obstacles</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Twenty spots along the track each get a random obstacle when the server starts, so every race is
	different.
</p>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"
			><tr><th class="px-4 py-3">Obstacle</th><th class="px-4 py-3">What it does</th></tr></thead
		>
		<tbody>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Knife</td><td class="px-4 py-3"
					>Spins and slides across the lane. Touching it sends you back to the start.</td
				></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Spatula</td><td class="px-4 py-3"
					>Slaps down on the track every 1 to 3 seconds. If it hits you, you are stunned for 2
					seconds: you cannot run nor jump.</td
				></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3">Broom</td><td class="px-4 py-3"
					>Spins along the floor. If it hits you, you run at half speed for 3 seconds.</td
				></tr
			>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Cheese</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Four pieces of cheese float above the track. Run into one to eat it: you run
	<strong>twice as fast for 2 seconds</strong>, and the camera pulls back while the boost lasts. A
	new piece of cheese appears somewhere else 5 seconds later.
</p>

<h2 class="mt-9 text-lg font-semibold">Mouse traps</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Every rat has <strong>3 traps</strong> per race.</li>
	<li>
		Press <code>E</code> to throw one in front of you: it slides along the floor the way your rat
		faces, then disappears after 6 seconds.
	</li>
	<li>A rat caught by a trap goes back to the start, and the trap is gone.</li>
	<li>
		Your own trap cannot catch you during its first 5 seconds, so you can throw it and run after it.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Tips</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Watch a spatula's rhythm before running under it.</li>
	<li>A cheese boost starts as soon as you eat it: grab the ones before a straight line.</li>
	<li>Throw traps where the track narrows, just ahead of a rival.</li>
</ul>
