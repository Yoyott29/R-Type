<svelte:head><title>Gameplay - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Gameplay</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Waves, enemies, bullets, and the play field</p>
<h2 class="mt-9 text-lg font-semibold">Objective</h2>
<p class="mt-3 max-w-[100ch] leading-7">Survive enemy waves and destroy enemies with bullets. The server owns movement, collisions, spawning, and elimination.</p>
<h2 class="mt-9 text-lg font-semibold">Enemy waves</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Each wave contains a random number of enemies.</li>
	<li>One enemy type is selected for the entire wave.</li>
	<li>Wave type 1 uses <code>Bydos1.png</code>; wave type 2 uses <code>Bydos2.png</code>.</li>
	<li>Enemies enter from the right and follow sinusoidal movement.</li>
</ul>
<h2 class="mt-9 text-lg font-semibold">Collisions</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>A bullet hitting an enemy destroys both entities.</li>
	<li>An enemy touching a player disconnects that player.</li>
	<li>Off-screen entities are queued for destruction.</li>
	<li>Stars are background entities and render behind gameplay entities.</li>
</ul>
