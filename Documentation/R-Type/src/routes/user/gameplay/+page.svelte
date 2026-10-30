<svelte:head><title>Gameplay - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Gameplay</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Waves, enemies, bullets, and the play field</p>
<h2 class="mt-9 text-lg font-semibold">Objective</h2>
<p class="mt-3 max-w-[100ch] leading-7">Survive enemy waves and destroy enemies with bullets. The server owns movement, collisions, spawning, and elimination.</p>
<h2 class="mt-9 text-lg font-semibold">The play field</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The field is 1920 by 1080 pixels, scaled down to fit the window on smaller monitors.</li>
	<li>White stars scroll from right to left in the background, at 100 pixels per second. A new star appears every half second.</li>
	<li>Each of the four players has its own ship color: green, blue, magenta and red (see <a href="/user/multiplayer" class="text-[#ff8a3d] hover:underline">Multiplayer</a>).</li>
</ul>
<h2 class="mt-9 text-lg font-semibold">Your ship</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>It moves at 400 pixels per second and cannot leave the screen.</li>
	<li>It fires on its own: one bullet every 0.2 s, flying right at 550 pixels per second.</li>
</ul>
<h2 class="mt-9 text-lg font-semibold">Enemy waves</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>A wave starts after a random delay between 0.5 and 1.4 seconds.</li>
	<li>Each wave contains 5 to 10 enemies, one every 0.2 s, all at the same random height.</li>
	<li>One enemy type is selected for the entire wave: <code>Bydos1.png</code> or <code>Bydos2.png</code>.</li>
	<li>Enemies enter from the right at 500 pixels per second and follow sinusoidal movement.</li>
	<li>Enemies do not shoot yet.</li>
</ul>
<h2 class="mt-9 text-lg font-semibold">Collisions</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>A bullet hitting an enemy destroys both entities.</li>
	<li>An enemy touching a player eliminates that player: its client stops with "Player eliminated by the server".</li>
	<li>Bullets, stars and other players never hurt a player.</li>
	<li>Off-screen entities are queued for destruction.</li>
	<li>Stars are background entities and render behind gameplay entities.</li>
</ul>
<h2 class="mt-9 text-lg font-semibold">Not in the game yet</h2>
<p class="mt-3 max-w-[100ch] leading-7">This prototype has no lives, score, power-ups, game over screen, sound or music. A player that is eliminated starts the client again to rejoin.</p>
