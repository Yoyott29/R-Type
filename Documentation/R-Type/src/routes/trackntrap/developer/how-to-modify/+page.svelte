<script>
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';
	const winTnt = `& ".\\Gringo Engine\\Gringo.exe" --build "Gringo Engine\\TrackNTrap\\TrackNTrap.gringo"`;
	const code0 = `./Gringo\\ Engine/Gringo --build "Gringo Engine/TrackNTrap/TrackNTrap.gringo"`;
</script>

<svelte:head>
	<title>How to Modify - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">How to Modify</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Recipes for the common changes</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Script parameters are changed in the editor: open <code>TrackNTrap.gringo</code> with <strong>Load Game</strong>, select the entity or prefab, and edit the values under its Script component. After any change, build again and commit the regenerated mains:
</p>

<Platform os="windows">
	<p class="mt-3 leading-7">
		From a <strong>Developer PowerShell for VS 2022</strong>, at the root of the repository (the editor calls CMake and MSVC, so it needs that terminal):
	</p>
	<pre class={pre}><code>{winTnt}</code></pre>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{code0}</code></pre>
</Platform>

<h2 class="mt-9 text-lg font-semibold">Tune the race</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">To change</th><th class="px-4 py-3">Edit</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Number of laps, countdown, time before the server closes</td><td class="px-4 py-3 align-top">Race Director: <code>laps</code>, <code>countdown</code>, <code>endDelay</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Rat speed, jump, cheese boost</td><td class="px-4 py-3 align-top">Rat1 to Rat4: RatPlayerController <code>speed</code>, <code>jump_force</code>, <code>boost_speed</code>, <code>boost_time</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Number of traps, their speed, life and immunity</td><td class="px-4 py-3 align-top">Rat1 to Rat4: TrapPlacer parameters</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Camera distance, angle, mouse sensitivity</td><td class="px-4 py-3 align-top">Rat Camera: RatCamera parameters</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Mix of obstacles</td><td class="px-4 py-3 align-top">Obstacles: <code>KnifeWeight</code>, <code>SpatulaWeight</code>, <code>BroomWeight</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Stun or slow duration, knife and broom speed, spatula rhythm</td><td class="px-4 py-3 align-top">The Knife, Spatula and Broom prefabs</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Number of cheese pieces, respawn time, float and spin</td><td class="px-4 py-3 align-top">Cheeses: CheeseSpawner parameters</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Gravity</td><td class="px-4 py-3 align-top">Settings panel: Physics3D <code>GravityY</code></td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Change or add a key</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Change a key: edit <code>bindKeys</code> in <code>RatCamera.hpp</code>.</li>
	<li>Add an action: add an <code>ACTION_...</code> number in <code>Net.hpp</code> (below 16: bits 16 to 24 carry the camera angle), bind a key to it in <code>bindKeys</code>, then read it on the server with <code>input.isActive(ACTION_...)</code> from the rat's <code>RemoteInput</code>, as <code>TrapPlacer</code> does with E.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Add an obstacle</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Import its model in <code>assets/</code>.</li>
	<li>Create a prefab in the editor: 3DModel, a static RigidBody3D, a Collider3D with <code>IsTrigger</code> on, and a <code>Networked</code> component in Replicated mode.</li>
	<li>If it moves, write a script for it (server side only, like <code>BroomSpin</code>) and add it to the prefab. Start with <code>if (Net::isClient(scene)) return;</code> in <code>onStart</code> and <code>onUpdate</code>.</li>
	<li>Choose its effect: nothing to do to send rats back to the start; or, in its script's <code>onStart</code>, register it in <code>Hazards</code> with a Stun or a Slow.</li>
	<li>Place it: add a weight and a branch for it in <code>ObstacleSpawner::onStart</code>, or put copies directly in the scene.</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">Add a new kind of effect</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Add the effect to <code>Hazard::Effect</code> in <code>Hazards.hpp</code>.</li>
	<li>Handle it in <code>RatPlayerController::suffer</code> (store a timer), count the timer down in <code>updateServer</code> and use it where the speed or the jump is computed.</li>
	<li>If the clients must show it, add a field to <code>RatStatus</code> and to both functions of its <code>NetCodec</code> in <code>Net.hpp</code>, in the same order.</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">Change the track</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The visible track is the Track model; the rats run on the Floor colliders and are held in by the Track Walls (<code>ArcWall3D</code>). Move them together.</li>
	<li>Update the obstacle spots (<code>SPOTS</code> in <code>ObstacleSpawner.hpp</code>) and the cheese spots (<code>SPOTS</code> in <code>CheeseSpawner.hpp</code>).</li>
	<li>Update the finish line and the checkpoints (<code>RACE_FINISH_...</code>, <code>RACE_EAST_X</code>, <code>RACE_NORTH_Z</code>, <code>RACE_WEST_X</code>) at the top of <code>RaceDirector.hpp</code>, and the lobby camera's center (CameraOrbit).</li>
	<li>The rats start where the Rat1 to Rat4 prefabs are placed: move their Transform, and set the Rat Camera's <code>start_angle</code> to the direction of the race.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Add a sound or a HUD element</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Create the entity in the editor (an AudioSource with <code>PlayOnStart</code> off, or a Text / Button) in <code>Client</code> mode.</li>
	<li>Give its editor id to the script that uses it, as a parameter, like <code>hit_sound</code> or the Hud's <code>timer</code>.</li>
	<li>In the script, find it with <code>EditorEntityIds</code> and change it from the client side (see <a href="/trackntrap/developer/architecture" class="text-accent hover:underline">Architecture</a>).</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Add a player</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	The game is made for four players: the network plugin accepts at most 4 clients, and there are four Rat prefabs and four lobby slots. A fifth would need a Rat5 prefab, a fifth slot text in the lobby and its id in the LobbyMenu's <code>slots</code>, Rat5 in the Network <code>PlayerPrefabs</code> setting, and a higher player limit in the engine's network plugin.
</p>

<h2 class="mt-9 text-lg font-semibold">Check your change</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Build with the command above: a script error stops the build with the compiler message.</li>
	<li>Start a server and two clients to see both sides: most bugs come from code running on the wrong side, so check the <code>Net::isClient</code> tests first.</li>
	<li>Format your code with the project's clang-format: the CI checks every C++ file.</li>
</ul>
