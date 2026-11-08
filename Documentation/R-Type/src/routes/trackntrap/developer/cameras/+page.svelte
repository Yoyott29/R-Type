<svelte:head>
	<title>Cameras - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Cameras</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">RatCamera follows your rat; CameraOrbit turns around the track in the lobby</p>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">RatCamera</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/RatCamera.hpp</code> &middot; on <strong>Rat Camera</strong> &middot; runs on <strong>the client</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>distance</code></td><td class="px-4 py-3 align-top">17</td><td class="px-4 py-3 align-top">Distance from the rat</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>pitch</code></td><td class="px-4 py-3 align-top">20</td><td class="px-4 py-3 align-top">Starting angle above the rat, in degrees (kept between 5 and 80)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>target_height</code></td><td class="px-4 py-3 align-top">1</td><td class="px-4 py-3 align-top">Height above the rat the camera looks at</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>sensitivity</code></td><td class="px-4 py-3 align-top">0.1</td><td class="px-4 py-3 align-top">Degrees per pixel of mouse movement</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>boost_distance</code></td><td class="px-4 py-3 align-top">1.3</td><td class="px-4 py-3 align-top">Distance multiplier while boosted: the camera pulls back</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>start_angle</code></td><td class="px-4 py-3 align-top">90</td><td class="px-4 py-3 align-top">Starting direction, the way the race goes from the start line</td></tr>
		</tbody>
	</table>
</div>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>Keys</strong>: <code>bindKeys</code> creates the InputBindings (Z, S, Space, E). This is the place to change or add keys.</li>
	<li>While the rat can move, the mouse turns the camera (horizontal angle) and tilts it (pitch).</li>
	<li>The distance eases towards <code>distance x boost_distance</code> while boosted, back to <code>distance</code> after.</li>
	<li>When the rat respawns, the camera goes back to its starting angles.</li>
	<li>Each frame it writes its horizontal angle into the input with <code>Net::setCameraAngle</code>: that is how the server knows which way Z goes.</li>
</ul>

<h3 class="mt-6 font-mono text-base font-semibold text-accent">CameraOrbit</h3>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>scripts/CameraOrbit.hpp</code> &middot; on <strong>Camera</strong> &middot; runs on <strong>the client</strong>
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Parameter</th><th class="px-4 py-3">In the project</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>CenterX</code>, <code>CenterZ</code></td><td class="px-4 py-3 align-top">-157, 222</td><td class="px-4 py-3 align-top">The point it turns around</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Radius</code></td><td class="px-4 py-3 align-top">0</td><td class="px-4 py-3 align-top">Distance of the camera from that point (0: it stays in the middle)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>LookRadius</code></td><td class="px-4 py-3 align-top">150</td><td class="px-4 py-3 align-top">Distance of the point it looks at</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Height</code></td><td class="px-4 py-3 align-top">40</td><td class="px-4 py-3 align-top">Camera height</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>Speed</code></td><td class="px-4 py-3 align-top">6</td><td class="px-4 py-3 align-top">Degrees per second</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	Turns while <code>GameState.playing</code> is false, so in the lobby only; the LobbyMenu then switches to the Rat Camera.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/trackntrap/developer/traps" class="text-accent hover:underline">Mouse Traps</a>.
</p>
