<svelte:head>
	<title>Accessibility - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Accessibility</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">What the game and this documentation offer to players with disabilities</p>

<p class="mt-3 max-w-[100ch] leading-7">
	This page follows the three usual categories: physical and motor, audio and visual, mental and cognitive. For each one it lists what the game offers.
</p>

<h2 class="mt-9 text-lg font-semibold">Physical and motor</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Firing is automatic: there is no button to hammer, only the four directions.</li>
	<li>Two sets of keys move the ship: the arrow keys, and a letter cluster for the other hand (see <a href="/user/controls" class="text-accent hover:underline">Controls</a>).</li>
	<li>The charged shot is optional: the game can be played without ever holding Space.</li>
	<li><strong>Key remapping</strong>: every movement key and the charged shot key can be changed in the Settings panel of the lobby, to fit one hand or another keyboard (see <a href="/user/controls" class="text-accent hover:underline">Controls</a>).</li>
	<li>No combination of keys is needed. The lobby buttons are large and need a single click.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Audio and visual</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>Color-blind filters</strong>: the <code>COLORS</code> button of the Settings panel switches between normal colors and protanopia, deuteranopia and tritanopia filters.</li>
	<li>No information is given only by sound: the only audio is the music, so deaf and hard of hearing players miss nothing.</li>
	<li>Ships, enemies and shots are bright sprites on a black background, with high contrast; destroyed enemies show an explosion.</li>
	<li>The lobby lists the players by number (P1 to P4), not only by color.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Mental and cognitive</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>One simple rule: avoid the enemies and their shots, your ship shoots for you.</li>
	<li>The lobby says who is the host and which slots are free; the score is always on screen.</li>
	<li>No time limit and no text to read during the match.</li>
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
