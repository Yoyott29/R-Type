<svelte:head>
	<title>About - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">About</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">The engine, its editor and the team behind them</p>

<p class="max-w-[100ch] leading-7">
	Gringo is a C++ game engine built around an <strong>Entity Component System</strong> (ECS), with an
	editor to create games without writing the engine code yourself. It was written for the Epitech
	R-Type project, then used to make a second, very different game.
</p>

<h2 class="mt-9 text-lg font-semibold">What it gives you</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>
		<strong>An editor</strong>: create a project, add entities and prefabs, set their components,
		choose plugins, and build the game in one click.
	</li>
	<li>
		<strong>Plugins</strong>: each feature is a separate library loaded when the game starts, so a
		game only carries what it uses: 2D, 3D, Physics2D, Physics3D, InputHandler, Scripts, Audio, UI
		and Network.
	</li>
	<li>
		<strong>Renderers</strong>: the drawing is done by a renderer plugin (raylib today), so games
		never call the graphics library directly.
	</li>
	<li>
		<strong>Networking</strong>: with the Network plugin, a game is built as a server and a client,
		with an authoritative server and up to four players over UDP.
	</li>
	<li>
		<strong>C++ scripts</strong>: the gameplay of each entity is a small C++ class with
		<code>onStart</code> and <code>onUpdate</code>, whose parameters are edited in the editor.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Games made with Gringo</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>
		<a href="/user/about" class="text-accent hover:underline">R-Type</a>: a 2D multiplayer
		side-scrolling shoot 'em up.
	</li>
	<li>
		<a href="/trackntrap/about" class="text-accent hover:underline">TrackNTrap</a>: a 3D multiplayer
		race between rats, with obstacles and traps.
	</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	The engine knows nothing about either game: everything specific to a game lives in its own
	project folder.
</p>

<h2 class="mt-9 text-lg font-semibold">The team</h2>
<ul class="mt-3 list-disc space-y-1 pl-6 text-[15px]">
	<li>Duchene Eliott: Engine, Game</li>
	<li>Vignes Arthur: Engine, Game</li>
	<li>Nedelec Clovis: Network</li>
	<li>Piron Arthur: Network</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Links</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Source code and issue tracker:
	<a
		href="https://github.com/EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4"
		class="text-accent underline-offset-2 hover:underline">GitHub repository</a
	>. The engine lives in <code>Gringo Engine/</code>.
</p>
