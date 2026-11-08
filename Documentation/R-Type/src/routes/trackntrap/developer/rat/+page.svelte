<script>
	const code0 = `void suffer(Hazard hazard)
{
    if (hazard.effect == Hazard::Effect::Stun) {
        if (_stun > 0.0f)      // a stun does not grow while it lasts
            return;
        _stun = hazard.duration;
        _hit = true;
        return;
    }
    if (_slow <= 0.0f)
        _hit = true;
    _slow = hazard.duration;   // a slow starts again from the full time
    _slowFactor = hazard.factor;
}`;
</script>

<svelte:head>
	<title>The Rat - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">The Rat</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">RatPlayerController: movement, hits and boost on the server; looks and sounds on the client</p>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/RatPlayerController.hpp</code> &middot; on <strong>Rat1 to Rat4 (prefabs)</strong> &middot; runs on <strong>both sides</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>speed</code></td><td class="px-4 py-3 align-top">60</td><td class="px-4 py-3 align-top">Running speed</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>jump_force</code></td><td class="px-4 py-3 align-top">20</td><td class="px-4 py-3 align-top">Upward speed given by a jump</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>boost_speed</code></td><td class="px-4 py-3 align-top">2</td><td class="px-4 py-3 align-top">Speed multiplier while a cheese boost runs</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>boost_time</code></td><td class="px-4 py-3 align-top">2</td><td class="px-4 py-3 align-top">Seconds a cheese boost lasts</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>hit_sound</code></td><td class="px-4 py-3 align-top">47</td><td class="px-4 py-3 align-top">Sound played when the rat is hit</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>running_sound</code></td><td class="px-4 py-3 align-top">48</td><td class="px-4 py-3 align-top">Looping sound while the rat runs</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	Change a value on all four prefabs, or the rats will not be equal.
</p>

<h2 class="mt-9 text-lg font-semibold">Server side: the real rat</h2>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Hits</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	The rat's collider is solid, but every obstacle, cheese and trap is a trigger: touching one calls the rat's <code>onCollision</code>, set in <code>startServer</code>. <code>touched</code> decides what happens, in this order:
</p>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>A <strong>cheese</strong> still on the track (the <code>Cheeses</code> resource says so): it is eaten, the boost timer starts.</li>
	<li>A <strong>trap the rat threw</strong>, during its immunity: nothing.</li>
	<li>Any other <strong>flying trap</strong>: the trap is spent (removed from <code>Traps</code>, put on layer 0).</li>
	<li>An obstacle in the <strong>Hazards</strong> resource: <code>suffer</code> applies its effect (stun or slow).</li>
	<li>Anything else (a knife, a trap): <code>dead = true</code>, the rat goes back to the start on its next update.</li>
</ol>

<p class="mt-3 max-w-[100ch] leading-7">
	So a new trigger obstacle sends rats back to the start by default; registering it in <code>Hazards</code> gives it a softer effect (see <a href="/trackntrap/developer/obstacles" class="text-accent hover:underline">Obstacles</a>).
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">Movement, every tick</h3>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Stands still while <code>GameState.canMove</code> is false (lobby, countdown, end).</li>
	<li>If <code>dead</code>: back to the spawn position, timers cleared, <code>respawns</code> and <code>hits</code> grow.</li>
	<li>Counts down the stun, slow and boost timers.</li>
	<li>Reads the player's keys and camera angle from <code>RemoteInput</code>. Z moves towards the camera's direction, S away from it; the speed is multiplied by the boost and the slow; the rat turns to face where it runs.</li>
	<li>Jumps on the press of Space (not while held), only when the RigidBody3D says it is grounded and the rat is not stunned.</li>
	<li>Resizes its box collider to the rat's rotation (<code>fitCollider</code>), since the box is aligned with the world axes.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Client side: the copy of the rat</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The server moves the rat, so the client makes its copy static, with no gravity.</li>
	<li><strong>Animation</strong>: running plays the model's animation; standing still holds frame 0.58, a pose with the feet on the ground (<code>STANDING_FRAME</code>).</li>
	<li><strong>Shadow</strong>: a flattened dark sphere created under the rat, kept on the floor while it jumps, so players see where it lands.</li>
	<li><strong>Dust</strong>: the prefab's ParticleEmitter3D is moved to two new entities, one under each back foot. They emit while the rat runs on the floor, twice as dense and blue while boosted.</li>
	<li><strong>Colored box</strong>: <code>createBox</code> adds a cube entity with <code>ChildOf</code> the rat and a <code>Transform</code> relative to it (above the middle of the body). The hierarchy moves the box with the rat and destroys it with it. Its color comes from the rat's slot (<code>BOX_COLOR_1</code> to <code>4</code>), its size from <code>BOX_SIZE</code> and <code>BOX_GAP</code>.</li>
	<li><strong>Sounds</strong>, for this player's rat only: the running loop while running, the hit sound each time <code>RatStatus.hits</code> grows.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/trackntrap/developer/cameras" class="text-accent hover:underline">Cameras</a>.
</p>
