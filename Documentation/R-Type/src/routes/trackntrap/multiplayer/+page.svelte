<svelte:head>
	<title>Multiplayer - TrackNTrap docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">Multiplayer</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	Racing with networked clients: the server runs the race, clients show it and send input
</p>

<p class="max-w-[100ch] leading-7">
	TrackNTrap uses the network plugin of the Gringo Engine, like R-Type. The server decides
	everything (where every rat is, who touched what, who won); each client shows the race and sends
	its player's keys.
</p>

<h2 class="mt-9 text-lg font-semibold">Host a race</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>
		Start one server. By default it only listens on <code>127.0.0.1</code>: give it
		<code>0.0.0.0</code> (or the host's LAN address) to accept other machines.
	</li>
	<li>Allow UDP on the chosen port in the host's firewall.</li>
	<li>Start one client per player, with the host's address and port. At most four clients.</li>
</ul>
<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>host      ./Gringo\ Engine/TrackNTrap/trackntrap_server 0.0.0.0 4242
player    ./Gringo\ Engine/TrackNTrap/trackntrap_client 192.168.1.20 4242     # the LAN address of the host</code></pre>
<p class="mt-3 max-w-[100ch] leading-7">
	The arguments are <code>[ip] port</code>; without them, both programs use
	<code>127.0.0.1:4242</code>.
</p>

<h2 class="mt-9 text-lg font-semibold">The lobby</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>
		Every client opens on the lobby, with the camera turning around the track. The four slots show
		which rats are taken (<code>HOST</code>, <code>READY</code>) or free (<code>EMPTY</code>).
	</li>
	<li>
		The first player to join is the <strong>host</strong>: the host sees "You are the host: click
		START when everybody is in", the others "Waiting for the host to start...".
	</li>
	<li>When the host clicks <strong>START</strong>, the countdown begins for everybody.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">One race per server</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>Once the race has started, the server refuses new players.</li>
	<li>
		When a rat wins, the result stays on screen for 5 seconds, then the server closes and every
		client is disconnected. Start the server again for a new race.
	</li>
	<li>If the host leaves, the server closes and the race ends for everybody.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Connection problems</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>
		Build the server and the clients from the same version of the project: if their network
		protocol versions differ, the server refuses the client.
	</li>
	<li>A fifth client is refused: the race is full.</li>
	<li>
		A client that hears nothing from the server for 5 seconds gives up; so does the server for that
		client.
	</li>
</ul>
