<svelte:head>
	<title>The built-in plugins - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">The built-in plugins</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">What each plugin of Gringo Engine/src/plugins brings</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Schedules, in the order of a frame: <code>PrepUpdate</code>, <code>StateTransition</code>, <code>FixedUpdate</code> (60 times per second, as many times as needed), <code>Update</code>, <code>PostUpdate</code>, then <code>Render</code> and <code>Overlay</code> when there is a renderer. See <a href="/gringo/developer/game-loop" class="text-accent hover:underline">How the game loop works</a>.
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Plugin</th><th class="px-4 py-3">Components</th><th class="px-4 py-3">Systems (schedule)</th><th class="px-4 py-3">Needs a renderer</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>2D</strong></td><td class="px-4 py-3 align-top"><code>Sprite</code>, <code>Animator</code>, <code>ParticleEmitter2D</code></td><td class="px-4 py-3 align-top">SpriteSystem, ParticleRender2DSystem (Render); AnimationSystem, ParticleEmitter2DSystem (FixedUpdate). Resource: <code>AnimationLibrary</code> (the <code>.anim</code> files)</td><td class="px-4 py-3 align-top">Yes</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>3D</strong></td><td class="px-4 py-3 align-top"><code>Camera</code>, <code>CameraFollow3D</code>, <code>3DModel</code>, <code>ParticleEmitter3D</code></td><td class="px-4 py-3 align-top">CameraSystem, Model3DSystem, ParticleRender3DSystem (Render); CameraFollowSystem (PostUpdate); ParticleEmitter3DSystem (FixedUpdate). Resource: <code>ActiveCamera3D</code></td><td class="px-4 py-3 align-top">Yes</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>Physics2D</strong></td><td class="px-4 py-3 align-top"><code>Velocity</code>, <code>Collider2D</code>, <code>RigidBody2D</code></td><td class="px-4 py-3 align-top">GravitySystem, MovementSystem, Collider2DSystem (FixedUpdate). Resource: <code>Gravity2D</code>. Settings: <code>GravityX</code>, <code>GravityY</code></td><td class="px-4 py-3 align-top">No</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>Physics3D</strong></td><td class="px-4 py-3 align-top"><code>Velocity3D</code>, <code>RigidBody3D</code>, <code>Collider3D</code>, <code>ArcWall3D</code></td><td class="px-4 py-3 align-top">Gravity3DSystem, Movement3DSystem, Collider3DSystem, ArcWall3DSystem (FixedUpdate). Resource: <code>Gravity3D</code>. Settings: <code>GravityX</code>, <code>GravityY</code>, <code>GravityZ</code></td><td class="px-4 py-3 align-top">No</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>InputHandler</strong></td><td class="px-4 py-3 align-top">None</td><td class="px-4 py-3 align-top">KeyboardSystem (PrepUpdate): fills <code>KeyboardInput</code> with the keys down and, if the game added <code>InputBindings</code>, turns them into the <code>ActionInput</code> resource</td><td class="px-4 py-3 align-top">Yes</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>Scripts</strong></td><td class="px-4 py-3 align-top"><code>Script</code></td><td class="px-4 py-3 align-top">ScriptSystem (FixedUpdate): calls <code>onStart</code> once, then <code>onUpdate</code> every tick. See <a href="/gringo/developer/scripts" class="text-accent hover:underline">Scripts</a></td><td class="px-4 py-3 align-top">No</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>Audio</strong></td><td class="px-4 py-3 align-top"><code>AudioSource</code></td><td class="px-4 py-3 align-top">AudioSystem (Update), playing through miniaudio</td><td class="px-4 py-3 align-top">Yes</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>UI</strong></td><td class="px-4 py-3 align-top"><code>Text</code> (UIText), <code>Button</code> (UIButton)</td><td class="px-4 py-3 align-top">TextSystem, ButtonSystem (Overlay); ButtonClickSystem (PrepUpdate): calls a button's script on click</td><td class="px-4 py-3 align-top">Yes</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><strong>Network</strong></td><td class="px-4 py-3 align-top"><code>Networked</code> (and the internal Mirror, NetId, RemoteInput)</td><td class="px-4 py-3 align-top">See <a href="/gringo/developer/network" class="text-accent hover:underline">How the network works</a>. Settings: <code>Host</code>, <code>Port</code>, <code>PlayerPrefabs</code>. Build variants: server and client</td><td class="px-4 py-3 align-top">No</td></tr>
		</tbody>
	</table>
</div>

<p class="mt-3 max-w-[100ch] leading-7">
	The plugins that need a renderer are left out of a headless server, along with their components: a server keeps the physics, the scripts and the network.
</p>

<p class="mt-3 max-w-[100ch] leading-7">
	Next: <a href="/gringo/developer/renderers" class="text-accent hover:underline">Renderers</a>.
</p>
