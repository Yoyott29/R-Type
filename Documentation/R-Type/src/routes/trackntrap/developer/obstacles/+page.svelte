<script>
	const code0 = `struct Hazard {
    enum class Effect { Slow, Stun };
    Effect effect;
    float duration;        // seconds
    float factor = 1.0f;   // Slow only: speed times this
};

scene.getResource<Hazards>().add(entity, {Hazard::Effect::Stun, _stunTime});`;
</script>

<svelte:head>
	<title>Obstacles - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Obstacles</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">ObstacleSpawner, Hazards, KnifeSpin, SpatulaSlap and BroomSpin</p>

<p class="mt-3 max-w-[100ch] leading-7">
	All the obstacles are moved by the server only: their Transform is replicated, so the clients see them move. Their collider is a trigger, handled by the rat (see <a href="/trackntrap/developer/rat" class="text-accent hover:underline">The Rat</a>).
</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">ObstacleSpawner</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/ObstacleSpawner.hpp</code> &middot; on <strong>Obstacles</strong> &middot; runs on <strong>the server</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>KnifeWeight</code></td><td class="px-4 py-3 align-top">1</td><td class="px-4 py-3 align-top">Chance of a knife, compared to the other weights</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>SpatulaWeight</code></td><td class="px-4 py-3 align-top">1</td><td class="px-4 py-3 align-top">Chance of a spatula</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>BroomWeight</code></td><td class="px-4 py-3 align-top">1</td><td class="px-4 py-3 align-top">Chance of a broom</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>SPOTS</code> lists 20 places along the track. Each spot has a position and <code>across</code>, the direction across the lane in degrees. When the server starts, each spot gets one obstacle picked with the weights (all at 1: one chance in three each):
</p>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>Knife</strong>: lowered by 1.9, and told to slide across the lane (its <code>MoveAngle</code> is set from <code>across</code>).</li>
	<li><strong>Spatula</strong>: placed 11.5 to one side of the lane, facing across it, on a random side.</li>
	<li><strong>Broom</strong>: in the middle of the lane, turned across it.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	A weight of 0 removes an obstacle type. To move or add spots, edit <code>SPOTS</code>.
</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Hazards</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>Hazards.hpp</code>, a resource of the server: the obstacles that do something else than sending the rat back to the start. Each obstacle script registers its entity on start:
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<p class="mt-3 max-w-[100ch] leading-7">
	The knife registers nothing: it sends rats back to the start.
</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">KnifeSpin</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/KnifeSpin.hpp</code> &middot; on <strong>Knife (prefab)</strong> &middot; runs on <strong>the server</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>RotationSpeed</code></td><td class="px-4 py-3 align-top">360</td><td class="px-4 py-3 align-top">Highest spin speed on each axis, in degrees per second</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>MoveSpeed</code></td><td class="px-4 py-3 align-top">15</td><td class="px-4 py-3 align-top">Sliding speed</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>MoveRange</code></td><td class="px-4 py-3 align-top">10</td><td class="px-4 py-3 align-top">How far it slides on each side of its spot</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>MoveAngle</code></td><td class="px-4 py-3 align-top">0</td><td class="px-4 py-3 align-top">Sliding direction, set by the ObstacleSpawner</td></tr>
		</tbody>
	</table>
</div>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Every 1 to 2.5 seconds it picks a new random spin speed on each axis.</li>
	<li>It slides back and forth around its spot, starting at a random point and direction.</li>
	<li><code>turned</code> works out where the middle of the blade is after the rotation, so the knife spins around its middle instead of around the end of the model.</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">SpatulaSlap</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/SpatulaSlap.hpp</code> &middot; on <strong>Spatula (prefab)</strong> &middot; runs on <strong>both sides</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>MinDelay</code>, <code>MaxDelay</code></td><td class="px-4 py-3 align-top">1, 3</td><td class="px-4 py-3 align-top">Random wait, in seconds, between two slaps</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>SlapTime</code></td><td class="px-4 py-3 align-top">0.15</td><td class="px-4 py-3 align-top">Time to fall flat</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>HoldTime</code></td><td class="px-4 py-3 align-top">0.4</td><td class="px-4 py-3 align-top">Time it stays flat</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>RaiseTime</code></td><td class="px-4 py-3 align-top">1</td><td class="px-4 py-3 align-top">Time to stand up again</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>StunTime</code></td><td class="px-4 py-3 align-top">2</td><td class="px-4 py-3 align-top">Stun given to a rat it touches (registered in Hazards)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>BurstTime</code></td><td class="px-4 py-3 align-top">0.12</td><td class="px-4 py-3 align-top">Client: how long the dust bursts on impact</td></tr>
		</tbody>
	</table>
</div>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>Server</strong>: a small state machine, Waiting, Slapping, Holding, Raising, back to Waiting. <code>angle()</code> eases the fall (fast at the end) and the rise (slow at the end); <code>apply</code> sets <code>rotationX</code> from it.</li>
	<li><strong>Client</strong>: it receives the rotation, and when the spatula becomes flat (<code>rotationX</code> at or below -85) it bursts dust under the head, from the prefab's ParticleEmitter3D moved to an entity of its own.</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">BroomSpin</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/BroomSpin.hpp</code> &middot; on <strong>Broom (prefab)</strong> &middot; runs on <strong>the server</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Speed</code></td><td class="px-4 py-3 align-top">200</td><td class="px-4 py-3 align-top">Degrees per second, in a random direction</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>FloorGap</code></td><td class="px-4 py-3 align-top">0.3</td><td class="px-4 py-3 align-top">Gap between the floor and the broom head</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>SlowTime</code></td><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">Seconds a rat it touches is slowed</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>SlowFactor</code></td><td class="px-4 py-3 align-top">0.5</td><td class="px-4 py-3 align-top">Speed of a slowed rat (0.5: half speed)</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	The broom turns around the top of its handle, starting at a random angle: <code>place</code> finds where the top is after the rotation, and the broom is moved so the top stays fixed.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/trackntrap/developer/cheese" class="text-accent hover:underline">Cheese</a>.
</p>
