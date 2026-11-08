<svelte:head>
	<title>Getting Started - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Getting Started</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Build the Gringo Engine and open its editor</p>

<p class="max-w-[100ch] leading-7">
	Gringo is the ECS game engine behind R-Type and TrackNTrap. It comes with an editor, a window where
	you create a game, fill it with entities and components, and build it into a program you can run.
	The engine is a standalone project: it knows nothing about the games made with it.
</p>

<h2 class="mt-9 text-lg font-semibold">Prerequisites</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A C++20 compiler, CMake 3.28 or newer, Git and, on Linux, the OpenGL and X11 development packages.
	See the <a href="/user/getting-started" class="text-accent hover:underline"
		>R-Type getting started</a
	>
	page to install them. The editor is used and tested on Linux.
</p>

<h2 class="mt-9 text-lg font-semibold">Build the engine and the editor</h2>
<p class="mt-3 max-w-[100ch] leading-7">From the root of the repository:</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./Gringo\ Engine/build_engine.sh</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">
	The first build downloads the dependencies (raylib, Asio, miniaudio, nlohmann/json) and takes a few
	minutes. It creates, in <code>Gringo Engine/</code>:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li><code>Gringo</code>: the editor.</li>
	<li><code>libgringo_engine.so</code>: the engine library every game uses.</li>
	<li>
		<code>plugins/</code> and <code>renderers/</code>: the engine features (2D, 3D, physics, audio,
		network, UI...) and the renderer, loaded by the games when they start.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Open the editor</h2>
<p class="mt-3 max-w-[100ch] leading-7">Start the editor from inside the <code>Gringo Engine</code> folder:</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>cd "Gringo Engine"
./Gringo</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">
	The folder matters: the editor looks for projects to load in the folder it is started from and in
	the folder above it. From <code>Gringo Engine</code>, it finds your own projects and TrackNTrap
	(saved in <code>Gringo Engine/</code>) as well as R-Type (in the repository root).
</p>

<h2 class="mt-9 text-lg font-semibold">The main menu</h2>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"
			><tr><th class="px-4 py-3">Button</th><th class="px-4 py-3">What it does</th></tr></thead
		>
		<tbody>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">New Game</td><td class="px-4 py-3"
					>Create a project from a 2D or 3D template</td
				></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">Load Game</td><td class="px-4 py-3"
					>Open an existing project (a <code>.gringo</code> file)</td
				></tr
			>
			<tr class="border-t border-[#1e2b26]"
				><td class="px-4 py-3 font-mono">Exit</td><td class="px-4 py-3">Close the editor</td></tr
			>
		</tbody>
	</table>
</div>
<p class="mt-3 max-w-[100ch] leading-7">
	Everything in the editor is done with the mouse (left click) and the keyboard to type names and
	values.
</p>

<h2 class="mt-9 text-lg font-semibold">Build a project without opening the editor</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The editor can also build a project straight from the terminal, without opening its window. Run it
	from the root of the repository with the path of the project file:
</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./Gringo\ Engine/Gringo --build R-Type/R-Type.gringo
./Gringo\ Engine/Gringo --build "Gringo Engine/TrackNTrap/TrackNTrap.gringo"</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">
	This is how TrackNTrap is built, and how the R-Type <code>client_main.cpp</code> and
	<code>server_main.cpp</code> are generated again after changing <code>R-Type.gringo</code>.
</p>

<p class="mt-9 max-w-[100ch] leading-7">
	Next: <a href="/gringo/creating-a-project" class="text-accent hover:underline"
		>Creating a Project</a
	>.
</p>
