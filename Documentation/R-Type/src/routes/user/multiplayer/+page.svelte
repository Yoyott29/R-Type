<svelte:head><title>Multiplayer - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Multiplayer</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Playing with multiple networked clients</p>
<h2 class="mt-9 text-lg font-semibold">Host a game</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Start one server on the host machine.</li>
	<li>Start one client per player.</li>
	<li>For remote players, configure the client with the host IP and UDP port.</li>
	<li>Use no more than four player clients.</li>
</ol>
<h2 class="mt-9 text-lg font-semibold">Authoritative server</h2>
<p class="mt-3 max-w-[100ch] leading-7">Clients send connection and input packets. The server sends snapshots containing positions, types, player ids, sprite data, and enemy variants. Clients render those snapshots rather than simulating authoritative gameplay locally.</p>
<h2 class="mt-9 text-lg font-semibold">Connection problems</h2>
<p class="mt-3 max-w-[100ch] leading-7">All clients and the server must use the same protocol version. A client can also be rejected when all four slots are occupied or when it has timed out.</p>
