<script>
	import Platform from '$lib/components/Platform.svelte';

	const pre =
		'mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6';
	const winCode = `host       .\\r-type_server.exe 0.0.0.0 4242
player     .\\r-type_client.exe 192.168.1.20 4242     # the LAN address of the host`;
	const code0 = `host       ./r-type_server 0.0.0.0 4242
player     ./r-type_client 192.168.1.20 4242     # the LAN address of the host`;
</script>

<svelte:head>
	<title>Multiplayer - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Multiplayer</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Playing with networked clients: the server runs the game, clients show it and send input</p>

<h2 class="mt-9 text-lg font-semibold">Host a game</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Start one server. By default it only listens on <code>127.0.0.1</code>: give it <code>0.0.0.0</code> (or the host's LAN address) to accept other machines.</li>
	<li>Allow UDP on the chosen port in the host's firewall.</li>
	<li>Start one client per player, with the host's address and port. At most four clients.</li>
</ul>

<Platform os="windows">
	<pre class={pre}><code>{winCode}</code></pre>
	<p class="mt-3 leading-7">
		When Windows Firewall asks the first time the server starts, allow it on private networks. To find the host's LAN address, run <code>ipconfig</code> and read the <strong>IPv4 Address</strong> line.
	</p>
</Platform>

<Platform os="linux">
	<pre class={pre}><code>{code0}</code></pre>
	<p class="mt-3 leading-7">
		To find the host's LAN address, run <code>hostname -I</code>. If a firewall such as <code>ufw</code> is active, open the port: <code>sudo ufw allow 4242/udp</code>.
	</p>
</Platform>

<p class="mt-3 max-w-[100ch] leading-7">
	The arguments are <code>[ip] port</code>; without them, both programs use <code>127.0.0.1:4242</code>.
</p>

<h2 class="mt-9 text-lg font-semibold">The lobby</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Every client opens on the lobby: the four slots show <code>HOST</code>, <code>READY</code> or <code>EMPTY</code>.</li>
	<li>The first player to join is the <strong>host</strong>. The match starts when the host clicks <strong>START</strong>; the other players' clicks do nothing.</li>
	<li>During a match, the server refuses new players. They can join again once everybody is back in the lobby.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Disconnections</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>When a player quits or crashes, the server removes its ship; if the client crashed, after 5 seconds without news from it.</li>
	<li>A player who left can join again: start the client again, it takes the first free slot.</li>
	<li>If the server stops, the clients go back to an empty lobby. They do not reconnect by themselves: start the server, then the clients again.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Connection problems</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The server and the clients must be built from the same version: if their network protocol versions differ, the server refuses the client.</li>
	<li>A fifth client is refused: the game is full.</li>
	<li>See <a href="/user/troubleshooting" class="text-accent hover:underline">Troubleshooting</a> for launch problems.</li>
</ul>

<p class="mt-3 max-w-[100ch] leading-7">
	How the network works inside is described in the <a href="/gringo/developer/network" class="text-accent hover:underline">Gringo developer documentation</a>.
</p>
