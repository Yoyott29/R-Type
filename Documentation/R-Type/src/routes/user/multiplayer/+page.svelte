<svelte:head><title>Multiplayer - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Multiplayer</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Playing with multiple networked clients</p>
<h2 class="mt-9 text-lg font-semibold">Host a game</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>
		Start one server on the host machine. By default it only listens on <code>127.0.0.1</code>, which
		other machines cannot reach: give it <code>0.0.0.0</code> (or the LAN address of the host) to accept
		remote players.
	</li>
	<li>Allow UDP on the chosen port in the host firewall.</li>
	<li>Start one client per player, with the address and port of the host.</li>
	<li>Use no more than four player clients.</li>
</ol>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>host       ./r-type_server 0.0.0.0 4242
player     ./r-type_client 192.168.1.20 4242     # the LAN address of the host</code></pre>
<h2 class="mt-9 text-lg font-semibold">The four players</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each player gets the smallest free slot, from 0 to 3. The slot decides the ship and where it appears:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Player 0: green ship, starts at the top.</li>
	<li>Player 1: blue ship, 100 pixels below.</li>
	<li>Player 2: magenta ship.</li>
	<li>Player 3: red ship, starts at the bottom.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	The client prints its slot when it joins: <code>Connected to 192.168.1.20:4242 as player 0</code>.
</p>
<h2 class="mt-9 text-lg font-semibold">Authoritative server</h2>
<p class="mt-3 max-w-[100ch] leading-7">Clients send connection and input packets. The server sends snapshots containing positions, types, player ids, sizes, shapes, colors, draw order and enemy variants. Clients render those snapshots rather than simulating gameplay locally.</p>
<h2 class="mt-9 text-lg font-semibold">Connection problems</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>All clients and the server must use the same protocol version, so build them from the same commit.</li>
	<li>A client is rejected with "server full" when the four slots are taken.</li>
	<li>
		A client that left, timed out or was eliminated cannot come back from the same address and port: it is
		also rejected with "server full". Restarting the client uses a new port, so it can join again.
	</li>
	<li>A client or a server that hears nothing from the other side for 2 seconds gives up.</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	See <a href="/user/troubleshooting" class="text-[#ff8a3d] hover:underline">Troubleshooting</a> for the messages
	the client prints.
</p>
