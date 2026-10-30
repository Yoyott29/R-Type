<svelte:head><title>Accessibility - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Accessibility</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">What the game and this documentation offer to players with disabilities, and what is still missing</p>

<p class="max-w-[100ch] leading-7">
	This page follows the three usual categories: physical and motor, audio and visual, mental and cognitive. For each one it lists
	what the prototype does today and what we plan to add, inspired by settings found in other games.
</p>

<h2 class="mt-9 text-lg font-semibold">Physical and motor</h2>
<h3 class="mt-4 font-semibold">Today</h3>
<ul class="mt-2 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Firing is automatic: there is no button to hammer or hold, only the four directions.</li>
	<li>Two sets of keys move the ship: the arrow keys, and a letter cluster for the other hand (see <a href="/user/controls" class="text-[#ff8a3d] hover:underline">Controls</a>).</li>
	<li>No combination of keys and no precise timing is needed.</li>
</ul>
<h3 class="mt-4 font-semibold">Missing</h3>
<ul class="mt-2 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Key remapping, and gamepad or one-switch support.</li>
	<li>A slower game speed or an option to make the ship invincible.</li>
	<li>A pause: the game keeps running on the server.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Audio and visual</h2>
<h3 class="mt-4 font-semibold">Today</h3>
<ul class="mt-2 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The game has no sound, so no information is given only by sound: deaf and hard of hearing players miss nothing.</li>
	<li>Ships, enemies and bullets are bright sprites on a black background, with high contrast.</li>
	<li>Each player also starts on its own row (player 0 at the top, player 3 at the bottom), and the client prints its player number in the terminal.</li>
</ul>
<h3 class="mt-4 font-semibold">Missing</h3>
<ul class="mt-2 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The four ships have the same shape and differ only by color (green, blue, magenta, red). Green and red are hard to tell apart for color-blind players: a number or a shape per player is planned.</li>
	<li>The window size is chosen at startup to fit the monitor, up to 1920 by 1080, and cannot be resized or zoomed while playing.</li>
	<li>No sound cues for blind or low-vision players, and no volume or subtitle settings once sound is added.</li>
	<li>The stars always scroll in the background; there is no option to turn them off.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Mental and cognitive</h2>
<h3 class="mt-4 font-semibold">Today</h3>
<ul class="mt-2 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>One simple rule: avoid the enemies, your ship shoots them for you.</li>
	<li>No menu, no text to read during the game, no time limit.</li>
	<li>The client prints a plain sentence when it stops (for example "Player eliminated by the server"), explained in <a href="/user/troubleshooting" class="text-[#ff8a3d] hover:underline">Troubleshooting</a>.</li>
</ul>
<h3 class="mt-4 font-semibold">Missing</h3>
<ul class="mt-2 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Difficulty levels, and a short in-game tutorial.</li>
	<li>A pause and a way to rejoin without restarting the client.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">This documentation</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Every page and sidebar entry is a real link, usable with the keyboard (Tab, Enter) and a screen reader, with a visible focus outline.</li>
	<li>A "Skip to content" link is the first element of each page, and the sidebar is a <code>nav</code> landmark.</li>
	<li>The page language is set to English, and headings follow a logical order.</li>
	<li>Text colors pass WCAG AA contrast on the dark background (body text 14.5:1, secondary text 5.7:1, links 8:1).</li>
	<li>Animations of the home page only play when the system does not ask for reduced motion.</li>
	<li>Diagrams have a text description for screen readers.</li>
	<li>Pages are short, one topic each, written in plain English.</li>
</ul>
