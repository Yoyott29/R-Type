<script>
	const code0 = `#define ACTION_FORWARD 0     // Z
#define ACTION_BACK 1        // S
#define ACTION_JUMP 2        // Space
#define ACTION_TRAP 3        // E
#define ACTION_START 4       // set by the lobby when the host clicks START`;
	const code1 = `static void setCameraAngle(ActionInput &input, float degrees)   // client
static float cameraAngle(const ActionInput &input)              // server`;
	const code2 = `struct GameState {
    bool playing = false;   // the race has started (countdown included)
    bool canMove = false;   // after GO and before the end
    bool boosted = false;   // client: my rat is boosted
    bool finished = false;  // a rat won
    int respawns = 0;       // client: times my rat went back to the start
};`;
</script>

<svelte:head>
	<title>Shared State - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Shared State</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Net.hpp, GameState.hpp and what the scripts share</p>

<h2 class="mt-9 text-lg font-semibold">Actions</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>Net.hpp</code> names the bits of the player's input. The client sets them from the keys (<code>RatCamera</code> binds them), the server reads them in the rat's <code>RemoteInput</code>:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<h2 class="mt-9 text-lg font-semibold">The camera angle in the input</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The rat runs where its player's camera looks, so the server needs that angle. It travels in the same input as the keys: bits 16 to 24 hold the angle in 512 steps (about 0.7 degree each).
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code1}</code></pre>

<h2 class="mt-9 text-lg font-semibold">RatStatus</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A component on every rat, written by the server and replicated to the clients:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Field</th><th class="px-4 py-3">Meaning</th><th class="px-4 py-3">Read by</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>lap</code></td><td class="px-4 py-3 align-top">Current lap, starts at 1</td><td class="px-4 py-3 align-top">Hud (<code>LAP 1/3</code>)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>trapsLeft</code></td><td class="px-4 py-3 align-top">Mouse traps the rat can still throw</td><td class="px-4 py-3 align-top">Hud</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>hits</code></td><td class="px-4 py-3 align-top">Grows on every hit (stun, slow, back to start)</td><td class="px-4 py-3 align-top">The rat's client side: plays the hit sound</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>respawns</code></td><td class="px-4 py-3 align-top">Grows each time the rat goes back to the start</td><td class="px-4 py-3 align-top">RaceDirector (resets the lap), RatCamera (resets the camera)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>boosted</code></td><td class="px-4 py-3 align-top">A cheese boost is running</td><td class="px-4 py-3 align-top">Dust color, camera zoom</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>moving</code>, <code>grounded</code></td><td class="px-4 py-3 align-top">Running, on the floor</td><td class="px-4 py-3 align-top">Animation, shadow, dust, running sound</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">RaceStatus</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A single component, on the RaceInfo entity created by the RaceDirector, replicated to every client:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Field</th><th class="px-4 py-3">Meaning</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>phase</code></td><td class="px-4 py-3 align-top"><code>RACE_LOBBY</code>, <code>RACE_COUNTDOWN</code>, <code>RACE_RUNNING</code> or <code>RACE_FINISHED</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>countdown</code></td><td class="px-4 py-3 align-top">Seconds left before GO</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>time</code></td><td class="px-4 py-3 align-top">Seconds since GO</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>winner</code></td><td class="px-4 py-3 align-top">Slot (1 to 4) of the winning rat</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>laps</code></td><td class="px-4 py-3 align-top">Laps to finish</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	Both components get a <code>NetCodec</code> in <code>Net.hpp</code>: the order in which <code>write</code> and <code>read</code> handle the fields must stay the same. <code>Net::replicateStatus</code> registers them with the network plugin; the RaceDirector (server) and the Hud (client) call it.
</p>

<h2 class="mt-9 text-lg font-semibold">GameState</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	A resource that every script can read, one per program. It is not sent: the server fills its own (RaceDirector), each client rebuilds its own from <code>RaceStatus</code> and its rat's <code>RatStatus</code> (Hud).
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code2}</code></pre>

<h2 class="mt-9 text-lg font-semibold">Helpers</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Function</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Net::isClient</code>, <code>Net::isServer</code></td><td class="px-4 py-3 align-top">Which program runs the script</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Net::isRat</code></td><td class="px-4 py-3 align-top">A player's rat: Networked, owned by a player, prefab name starting with <code>Rat</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Net::slot</code></td><td class="px-4 py-3 align-top">The slot of a rat, from its prefab name (<code>Rat3</code> is 3)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Net::hostPeer</code></td><td class="px-4 py-3 align-top">The host: the lowest peer id owning a rat, so the first player in</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Net::myPeer</code>, <code>Net::findMyRat</code></td><td class="px-4 py-3 align-top">Client: this player's id and rat</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Net::findRace</code></td><td class="px-4 py-3 align-top">Copies the RaceStatus, if it exists yet</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/trackntrap/developer/race-flow" class="text-accent hover:underline">Race Flow</a>.
</p>
