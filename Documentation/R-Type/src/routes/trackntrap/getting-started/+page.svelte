<svelte:head>
	<title>Getting Started - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Getting Started</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Build TrackNTrap and start your first race</p>

<p class="max-w-[100ch] leading-7">
	TrackNTrap is networked: one server runs the race, and every player starts a client. This page
	gets a race running on one machine; see
	<a href="/trackntrap/multiplayer" class="text-accent hover:underline">Multiplayer</a> to play
	across several.
</p>

<h2 class="mt-9 text-lg font-semibold">Prerequisites</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The same tools as R-Type: a C++20 compiler, CMake 3.28 or newer, Git and, on Linux, the OpenGL and
	X11 development packages. See the
	<a href="/user/getting-started" class="text-accent hover:underline">R-Type getting started</a>
	page to install them. The game is built and tested on Linux.
</p>

<h2 class="mt-9 text-lg font-semibold">Build</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	TrackNTrap is built by the Gringo editor. Run these commands from the root of the repository:
	the first one builds the engine and the editor, the second one builds the game.
</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./Gringo\ Engine/build_engine.sh
./Gringo\ Engine/Gringo --build "Gringo Engine/TrackNTrap/TrackNTrap.gringo"</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">
	The first build downloads the dependencies and takes a few minutes. It creates two programs in
	<code>Gringo Engine/TrackNTrap/</code>: <code>trackntrap_server</code> and
	<code>trackntrap_client</code>.
</p>

<h2 class="mt-9 text-lg font-semibold">Start a race</h2>
<p class="mt-3 max-w-[100ch] leading-7">Start the server in one terminal:</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./Gringo\ Engine/TrackNTrap/trackntrap_server</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">Then one client per player, each in its own terminal:</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./Gringo\ Engine/TrackNTrap/trackntrap_client</code></pre>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Start the server first: a client tries to connect for about 5 seconds, then gives up.</li>
	<li>
		Every client opens on the lobby. The first player to join is the host: the race starts when the
		host clicks <strong>START</strong>.
	</li>
	<li>Press <code>Esc</code> or close the window to quit a client.</li>
</ul>

<p class="mt-9 max-w-[100ch] leading-7">
	Next: learn the <a href="/trackntrap/controls" class="text-accent hover:underline">Controls</a>.
</p>
