<script>
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';
	const winTnt = `& ".\\Gringo Engine\\Gringo.exe" --build "Gringo Engine\\TrackNTrap\\TrackNTrap.gringo"`;
	const winServer = `& ".\\Gringo Engine\\TrackNTrap\\trackntrap_server.exe"`;
	const winClient = `& ".\\Gringo Engine\\TrackNTrap\\trackntrap_client.exe"`;
	const winEditor = `cmake -S "Gringo Engine" -B "Gringo Engine\\build" -A x64
cmake --build "Gringo Engine\\build" --config Release --target Gringo --parallel`;

	const linuxBuild = `./Gringo\\ Engine/build_engine.sh
./Gringo\\ Engine/Gringo --build "Gringo Engine/TrackNTrap/TrackNTrap.gringo"`;
	const linuxServer = `./Gringo\\ Engine/TrackNTrap/trackntrap_server`;
	const linuxClient = `./Gringo\\ Engine/TrackNTrap/trackntrap_client`;
</script>

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
	The same tools as R-Type, installed as explained in the WINDOWS and LINUX sections of the
	<a href="/user/getting-started" class="text-accent hover:underline">R-Type getting started</a>
	page.
</p>

<h2 class="mt-9 text-lg font-semibold">Build</h2>
<p class="mt-3 max-w-[100ch] leading-7">TrackNTrap is built by the Gringo editor.</p>

<Platform os="windows">
	<p class="mt-3 leading-7">
		From a <strong>Developer PowerShell for VS 2022</strong>, at the root of the repository: the first two commands build the engine
		and the editor, the last one builds the game.
	</p>
	<pre class={pre}><code>{winEditor}
{winTnt}</code></pre>
	<p class="mt-3 leading-7">
		The first build downloads the dependencies and takes a few minutes. It creates
		<code>trackntrap_server.exe</code> and <code>trackntrap_client.exe</code> in
		<code>Gringo Engine\TrackNTrap\</code>, with <code>gringo_engine.dll</code> next to them.
	</p>
</Platform>

<Platform os="linux">
	<p class="mt-3 leading-7">
		Run these commands from the root of the repository: the first one builds the engine and the
		editor, the second one builds the game.
	</p>
	<pre class={pre}><code>{linuxBuild}</code></pre>
	<p class="mt-3 leading-7">
		The first build downloads the dependencies and takes a few minutes. It creates two programs in
		<code>Gringo Engine/TrackNTrap/</code>: <code>trackntrap_server</code> and
		<code>trackntrap_client</code>.
	</p>
</Platform>

<h2 class="mt-9 text-lg font-semibold">Start a race</h2>

<Platform os="windows">
	<p class="mt-3 leading-7">
		Start the server in one PowerShell window (the <code>&amp;</code> is needed because the path holds a space):
	</p>
	<pre class={pre}><code>{winServer}</code></pre>
	<p class="mt-3 leading-7">Then one client per player, each in its own window:</p>
	<pre class={pre}><code>{winClient}</code></pre>
	<p class="mt-3 leading-7">When Windows Firewall asks the first time the server starts, allow it.</p>
</Platform>

<Platform os="linux">
	<p class="mt-3 leading-7">Start the server in one terminal:</p>
	<pre class={pre}><code>{linuxServer}</code></pre>
	<p class="mt-3 leading-7">Then one client per player, each in its own terminal:</p>
	<pre class={pre}><code>{linuxClient}</code></pre>
</Platform>

<ul class="mt-5 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
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
