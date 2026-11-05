<svelte:head><title>Multiplayer - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Multiplayer</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	Playing with networked clients: the server runs the game, clients show it and send input
</p>
<h2 class="mt-9 text-lg font-semibold">Build</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A project with the Network plugin builds a server and a client next to its project file:
</p>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>"Gringo Engine/Gringo" --build R-Type/R-Type.gringo     # gives r-type_server and r-type_client</code></pre>
<h2 class="mt-9 text-lg font-semibold">Host a game</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>
		Start one server. By default it only listens on <code>127.0.0.1</code>: give it
		<code>0.0.0.0</code> (or the host's LAN address) to accept other machines.
	</li>
	<li>Allow UDP on the chosen port in the host firewall.</li>
	<li>Start one client per player, with the host's address and port. At most four clients.</li>
</ol>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>host       ./r-type_server 0.0.0.0 4242
player     ./r-type_client 192.168.1.20 4242     # the LAN address of the host</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">
	The arguments are <code>[host] port</code>; without them the project's Host and Port settings are
	used (127.0.0.1:4242).
</p>
<h2 class="mt-9 text-lg font-semibold">Connection problems</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The server and clients must use the same protocol version: build them from the same commit.</li>
	<li>A fifth client is rejected as full.</li>
	<li>A client that hears nothing for 5 seconds gives up; so does the server for that client.</li>
	<li>A client that left can join again: it gets the lowest free slot.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	See <a href="/user/troubleshooting" class="text-[#ff8a3d] hover:underline">Troubleshooting</a> for
	launch problems.
</p>
