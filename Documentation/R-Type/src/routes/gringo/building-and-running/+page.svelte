<script>
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';
	const winCli = `& ".\\Gringo Engine\\Gringo.exe" --build "Gringo Engine\\MyGame\\MyGame.gringo"`;
	const winRun = `& ".\\Gringo Engine\\MyGame\\mygame.exe"`;
	const winRunNet = `& ".\\Gringo Engine\\MyGame\\mygame_server.exe"
& ".\\Gringo Engine\\MyGame\\mygame_client.exe"`;

	const linuxRun = `./Gringo\\ Engine/MyGame/mygame`;
	const linuxRunNet = `./Gringo\\ Engine/MyGame/mygame_server
./Gringo\\ Engine/MyGame/mygame_client`;
	const linuxCli = `./Gringo\\ Engine/Gringo --build "Gringo Engine/MyGame/MyGame.gringo"`;
</script>

<svelte:head>
	<title>Building and Running - Gringo docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Building and Running</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Turn your project into a game you can play</p>

<h2 class="mt-9 text-lg font-semibold">Build</h2>

<Platform os="windows">
	<p class="mt-3 leading-7">
		Start the editor from a <strong>Developer PowerShell for VS 2022</strong>: the build calls CMake and the MSVC compiler, which a
		regular PowerShell window does not know.
	</p>
	<ol class="mt-3 list-decimal space-y-1.5 pl-6 leading-7">
		<li>
			Click <strong>Build</strong> in the top bar. It is greyed out while a component's plugin is off
			or the renderer is missing.
		</li>
		<li>
			The editor saves the project and checks it. If a script or an asset is missing, the problem is
			printed in the terminal and the editor stays open.
		</li>
		<li>Otherwise the editor closes and the game is compiled in the terminal, in Release.</li>
	</ol>
</Platform>

<Platform os="linux">
<ol class="mt-3 list-decimal space-y-1.5 pl-6 leading-7">
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
</Platform>

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

<Platform os="windows">
	<p class="mt-3 leading-7">
		The programs end with <code>.exe</code>, and <code>gringo_engine.dll</code> is copied next to
		them. The <code>&amp;</code> is needed in PowerShell because the path holds a space:
	</p>
	<pre class={pre}><code>{winRun}</code></pre>
	<p class="mt-3 leading-7">A networked game: start the server first, then one client per player.</p>
	<pre class={pre}><code>{winRunNet}</code></pre>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{linuxRun}</code></pre>
	<p class="mt-3 leading-7">A networked game: start the server first, then one client per player.</p>
	<pre class={pre}><code>{linuxRunNet}</code></pre>
</Platform>

<h2 class="mt-9 text-lg font-semibold">Build from the terminal</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	You can also build a project without opening the editor, from the root of the repository.
</p>

<Platform os="windows">
	<p class="mt-3 leading-7">From a <strong>Developer PowerShell for VS 2022</strong>:</p>
	<pre class={pre}><code>{winCli}</code></pre>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{linuxCli}</code></pre>
</Platform>

<h2 class="mt-9 text-lg font-semibold">Change the game</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Open the editor again, click <strong>Load Game</strong>, pick the project, change it, and build it
	again.
</p>
