<svelte:head>
	<title>The Project File - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">The Project File</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">What TrackNTrap.gringo contains</p>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>TrackNTrap.gringo</code> is edited with the Gringo editor (open it with <strong>Load Game</strong>). It is JSON, so small changes can also be made by hand: each entity has an <code>id</code>, a <code>name</code> and a list of components with their properties.
</p>

<h2 class="mt-9 text-lg font-semibold">Plugin settings</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Plugin</th><th class="px-4 py-3">Setting</th><th class="px-4 py-3">Value</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Network</td><td class="px-4 py-3 align-top"><code>Host</code> / <code>Port</code></td><td class="px-4 py-3 align-top"><code>127.0.0.1</code> / <code>4242</code>, used when no address is given on the command line</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Network</td><td class="px-4 py-3 align-top"><code>PlayerPrefabs</code></td><td class="px-4 py-3 align-top"><code>Rat1,Rat2,Rat3,Rat4</code>: the prefab the server creates for the player of each slot</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Physics3D</td><td class="px-4 py-3 align-top"><code>GravityY</code></td><td class="px-4 py-3 align-top"><code>-9.81</code></td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Entities</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Entities</th><th class="px-4 py-3">What they are</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Lobby, Lobby Title, Lobby Subtitle, Lobby Start, Lobby Slot 1 to 4</td><td class="px-4 py-3 align-top">The lobby screen, children of Lobby (id 110). Lobby has the LobbyMenu script, Lobby Start the StartButton script</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Camera (4)</td><td class="px-4 py-3 align-top">The lobby camera, with CameraOrbit</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Rat Camera (7)</td><td class="px-4 py-3 align-top">The race camera, with RatCamera</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Track (3), Ground (5), Finish Line (10)</td><td class="px-4 py-3 align-top">Models only: what you see of the track</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Rat Floor (8), Track Floor (59) and Floor 1 to 39</td><td class="px-4 py-3 align-top">Invisible solid colliders the rats run on</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Track Walls (27) and its 12 children</td><td class="px-4 py-3 align-top"><code>ArcWall3D</code> walls along the inner and outer edges of the lanes and turns</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Stone Wall 1 to 10</td><td class="px-4 py-3 align-top">Copies of the Stone Wall prefabs: solid decor</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Countdown (11)</td><td class="px-4 py-3 align-top">The big countdown text, with CountdownScript and a beep sound</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Main Theme (46), Hit Sound (47), Running Sound (48)</td><td class="px-4 py-3 align-top">Sounds, played by scripts that know their ids</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Trap Panel, Trap Icon, Trap Count, Trap Key (100 to 103), Race Timer (105), Lap Count (106), Win Text (107)</td><td class="px-4 py-3 align-top">The HUD elements, filled by the Hud script</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Race Director (108)</td><td class="px-4 py-3 align-top">RaceDirector, server only</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">HUD (109)</td><td class="px-4 py-3 align-top">Hud, client only</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Obstacles (99), Cheeses (104)</td><td class="px-4 py-3 align-top">The spawners, server only</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Prefabs</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Prefab</th><th class="px-4 py-3">Components</th><th class="px-4 py-3">Created by</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Rat1 to Rat4</td><td class="px-4 py-3 align-top">3D model (Run.glb, animation 0), Velocity3D, RigidBody3D, solid Collider3D, ParticleEmitter3D (dust), Replicated, RatPlayerController and TrapPlacer scripts</td><td class="px-4 py-3 align-top">The server, one per player (PlayerPrefabs)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Knife, Spatula, Broom</td><td class="px-4 py-3 align-top">Model, static RigidBody3D, <strong>trigger</strong> Collider3D, Replicated, their script (the spatula also has an impact ParticleEmitter3D)</td><td class="px-4 py-3 align-top">ObstacleSpawner</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Cheese</td><td class="px-4 py-3 align-top">Model, static RigidBody3D, trigger Collider3D, Replicated</td><td class="px-4 py-3 align-top">CheeseSpawner</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">MouseTrap</td><td class="px-4 py-3 align-top">Model, static RigidBody3D, trigger Collider3D, Replicated</td><td class="px-4 py-3 align-top">TrapPlacer</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">RaceInfo</td><td class="px-4 py-3 align-top">Replicated only: RaceDirector adds the RaceStatus component</td><td class="px-4 py-3 align-top">RaceDirector</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Stone Wall, Stone Wall Side</td><td class="px-4 py-3 align-top">Model and a solid collider</td><td class="px-4 py-3 align-top">Placed in the scene</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Colliders</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><strong>Solid</strong> colliders (floors, walls, other rats) block a rat.</li>
	<li><strong>Triggers</strong> (obstacles, cheese, traps) let it through but call the rat's <code>onCollision</code>: that is where every hit is handled, see <a href="/trackntrap/developer/rat" class="text-accent hover:underline">The Rat</a>.</li>
	<li>All colliders use layer 1 and collide with every layer. A cheese that was eaten or a trap that caught a rat is put on layer 0 so it touches nothing more before it is destroyed.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/trackntrap/developer/shared-state" class="text-accent hover:underline">Shared State</a>.
</p>
