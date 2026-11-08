<script>
	const code0 = `RACE_LOBBY  --(host clicks START)-->  RACE_COUNTDOWN  --(3 s)-->  RACE_RUNNING
RACE_RUNNING  --(a rat finishes its last lap)-->  RACE_FINISHED  --(5 s)-->  server closes`;
	const code1 = `#define RACE_FINISH_X -150.0f      // the line, crossed going towards +x
#define RACE_FINISH_MIN_Z 10.0f     // the width of the line
#define RACE_FINISH_MAX_Z 60.0f
#define RACE_CHECKPOINTS 3
#define RACE_EAST_X 250.0f          // checkpoint 1: x above this
#define RACE_NORTH_Z 380.0f         // checkpoint 2: z above this
#define RACE_WEST_X -450.0f         // checkpoint 3: x below this`;
</script>

<svelte:head>
	<title>Race Flow - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Race Flow</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">From the lobby to the winner: RaceDirector, LobbyMenu, StartButton, CountdownScript, Hud</p>

<h2 class="mt-9 text-lg font-semibold">The phases</h2>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	How START reaches the server: the button adds a <code>StartRequest</code> resource on the client; while it exists, the host's LobbyMenu sets <code>ACTION_START</code> in the input; the server's RaceDirector sees that bit in the host rat's <code>RemoteInput</code>.
</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">RaceDirector</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/RaceDirector.hpp</code> &middot; on <strong>Race Director</strong> &middot; runs on <strong>the server</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>laps</code></td><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">Laps to finish</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>countdown</code></td><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">Seconds between START and GO</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>info</code></td><td class="px-4 py-3 align-top">RaceInfo</td><td class="px-4 py-3 align-top">The prefab created to carry the RaceStatus</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>endDelay</code></td><td class="px-4 py-3 align-top">5</td><td class="px-4 py-3 align-top">Seconds the result stays on screen before the server closes</td></tr>
		</tbody>
	</table>
</div>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>onStart</strong>: registers the replicated components, adds the GameState and Traps resources, creates the RaceInfo entity with a RaceStatus.</li>
	<li><strong>onUpdate</strong>, every tick: closes the server if the host left, ages the flying traps, then runs the current phase. LOBBY waits for the host's START and stops accepting players; COUNTDOWN counts down then sets <code>canMove</code>; RUNNING adds to the time and counts the laps; FINISHED waits <code>endDelay</code>, then sets <code>AppAlive</code> to false, which stops the server and disconnects every client.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	Counting laps: the track has 3 checkpoints, reached in order (east part, north part, west part), then the finish line. The positions are constants at the top of the file (comments added here):
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code1}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	A crossing only counts once all 3 checkpoints are reached, so cutting across does not count. When a rat respawns (its <code>respawns</code> changed), its checkpoints restart from 0. The first rat to cross the line at the end of its last lap is the winner.
</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">LobbyMenu</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/LobbyMenu.hpp</code> &middot; on <strong>Lobby</strong> &middot; runs on <strong>the client</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>menuCamera</code></td><td class="px-4 py-3 align-top">4</td><td class="px-4 py-3 align-top">The lobby camera, switched off at the start</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>ratCamera</code></td><td class="px-4 py-3 align-top">7</td><td class="px-4 py-3 align-top">The race camera, switched on at the start</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>startButton</code></td><td class="px-4 py-3 align-top">113</td><td class="px-4 py-3 align-top">The START button</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>subtitle</code></td><td class="px-4 py-3 align-top">112</td><td class="px-4 py-3 align-top">The text under the title</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>slots</code></td><td class="px-4 py-3 align-top">114,115,116,117</td><td class="px-4 py-3 align-top">The four slot texts</td></tr>
		</tbody>
	</table>
</div>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Every frame in the lobby: writes the subtitle (connecting, host, waiting), shows the START button to the host only (by giving it a size of 0 for the others), writes each slot (<code>HOST</code>, <code>READY</code>, <code>(YOU)</code>, <code>EMPTY</code>).</li>
	<li>Once <code>GameState.playing</code> is true: switches the cameras, locks the mouse cursor and destroys the Lobby entity; its children (title, slots, button) go with it.</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">StartButton</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/StartButton.hpp</code> &middot; on <strong>Lobby Start</strong> &middot; runs on <strong>the client</strong>
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	A button script (<code>IGringoButtonScript</code>): <code>onClick</code> only adds the <code>StartRequest</code> resource. The LobbyMenu does the rest.
</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">CountdownScript</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/CountdownScript.hpp</code> &middot; on <strong>Countdown</strong> &middot; runs on <strong>the client</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>seconds</code></td><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">Start value, replaced by the server's countdown as soon as it arrives</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>startSize</code></td><td class="px-4 py-3 align-top">700</td><td class="px-4 py-3 align-top">Font size of a number when it appears; it shrinks to the Text's own size</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>music</code></td><td class="px-4 py-3 align-top">46</td><td class="px-4 py-3 align-top">The music, stopped during the countdown and played from GO</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	It shows the server's <code>RaceStatus.countdown</code> rounded up, plays the beep (its own AudioSource) each time the number changes, shrinks and fades each number, then shows <code>GO</code> for one second and empties the text.
</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Hud</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/Hud.hpp</code> &middot; on <strong>HUD</strong> &middot; runs on <strong>the client</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>timer</code></td><td class="px-4 py-3 align-top">105</td><td class="px-4 py-3 align-top">Race time text</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>lap</code></td><td class="px-4 py-3 align-top">106</td><td class="px-4 py-3 align-top">Lap text</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>win</code></td><td class="px-4 py-3 align-top">107</td><td class="px-4 py-3 align-top">Winner text</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>traps</code></td><td class="px-4 py-3 align-top">100,101,102,103</td><td class="px-4 py-3 align-top">The buttons of the trap panel, hidden until the race starts</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>trapCount</code></td><td class="px-4 py-3 align-top">102</td><td class="px-4 py-3 align-top">The button showing <code>X3</code>, <code>X2</code>...</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>trapKey</code></td><td class="px-4 py-3 align-top">103</td><td class="px-4 py-3 align-top">The button showing the key, hidden once no trap is left</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	Each frame it copies <code>RaceStatus</code> and its rat's <code>RatStatus</code> into the client's <code>GameState</code> (the other client scripts read it from there), then writes the time (<code>m:ss.cc</code>), the lap, the traps left and, at the end, <code>YOU WIN!</code> or <code>RAT n WINS!</code>.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/trackntrap/developer/rat" class="text-accent hover:underline">The Rat</a>.
</p>
