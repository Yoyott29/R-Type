<svelte:head>
	<title>Building and Running - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Building and Running</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Turn your project into a game you can play</p>

<h2 class="mt-9 text-lg font-semibold">Build</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>
		Click <strong>Build</strong> in the top bar. It is greyed out while a component's plugin is off
		or the renderer is missing.
	</li>
	<li>
		The editor saves the project and checks it. If a script or an asset is missing, the problem is
		printed in the terminal and the editor stays open.
	</li>
	<li>Otherwise the editor closes and the game is compiled in the terminal.</li>
</ol>

<h2 class="mt-9 text-lg font-semibold">What you get</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The game is created in the project folder, named after the project in lowercase (spaces become
	<code>_</code>):
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Without the Network plugin: one program, for example <code>Gringo Engine/MyGame/mygame</code>.</li>
	<li>
		With the Network plugin: a server and a client, <code>mygame_server</code> and
		<code>mygame_client</code>. They take an optional IP address and port, like R-Type.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Run</h2>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./Gringo\ Engine/MyGame/mygame</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">A networked game: start the server first, then one client per player.</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./Gringo\ Engine/MyGame/mygame_server
./Gringo\ Engine/MyGame/mygame_client</code></pre>

<h2 class="mt-9 text-lg font-semibold">Build from the terminal</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	You can also build a project without opening the editor, from the root of the repository:
</p>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>./Gringo\ Engine/Gringo --build "Gringo Engine/MyGame/MyGame.gringo"</code></pre>

<h2 class="mt-9 text-lg font-semibold">Change the game</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Open the editor again, click <strong>Load Game</strong>, pick the project, change it, and build it
	again.
</p>
