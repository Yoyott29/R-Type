<svelte:head>
	<title>Gameplay - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Gameplay</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Waves, shots, power-ups and the score</p>

<h2 class="mt-9 text-lg font-semibold">Objective</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Together, destroy as many Bydos as possible: every enemy destroyed adds 100 points to the team score. The match ends when every ship has been destroyed.
</p>

<h2 class="mt-9 text-lg font-semibold">The play field</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The field is 1920 by 1080, scaled to fit the window.</li>
	<li>Stars scroll from right to left in the background.</li>
	<li>Each player has its own ship color: P1 blue, P2 green, P3 magenta, P4 red.</li>
	<li>The team score is shown in the top left corner.</li>
	<li>The stage music plays during the match.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Your ship</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>It moves at 400 pixels per second and cannot leave the screen.</li>
	<li>It fires on its own: a shot every 0.13 s, straight ahead.</li>
	<li><strong>Charged shot</strong>: hold Space to charge, release to fire. Every 0.3 s of charge adds a level, up to 5. A charged shot is bigger, goes through every enemy on its way, and destroys them all.</li>
	<li>A single hit destroys it: you are out until the match ends.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Enemies</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>They come in waves from the right: 5 to 10 enemies, one every 0.2 s, all at the same random height and of the same kind (two kinds of Bydos).</li>
	<li>They fly left at 500 pixels per second, on a wavy path.</li>
	<li>They shoot at the nearest ship: the first shot 1 s after they appear, then every 2 to 4 seconds.</li>
	<li>Touching an enemy or one of its shots destroys your ship.</li>
	<li>A destroyed enemy explodes.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Power-ups</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Every 3000 points, a <strong>POW armor</strong> carrier crosses the screen. Destroy it: it always drops a power-up. Other enemies drop one very rarely.</li>
	<li>The power-up drifts slowly to the left: fly into it to take it.</li>
	<li>It gives your ship <strong>diagonal shots</strong>: two more shots, up and down, for the rest of the match.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">End of the match</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>When every ship is destroyed, everybody goes back to the lobby.</li>
	<li>The lobby shows a <strong>scoreboard</strong> with the 5 best scores of the session. Each client keeps its own, until it is closed.</li>
	<li>The host can start a new match; players who joined meanwhile take part.</li>
</ul>
