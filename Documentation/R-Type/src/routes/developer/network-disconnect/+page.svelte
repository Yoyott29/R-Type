<script>
	const waysExample = `the client closes its window   -> Client::cleanup sends [DISCONNECT]
the client crashes or freezes  -> no packet for 2 s: the server drops it
the player hits an enemy       -> its collider callback eliminates it
the server stops or dies       -> no snapshot for 2 s: the client stops`;

	const leaveExample = `Network::disconnectPlayer(id)
  address -> _eliminatedPlayers       (it cannot come back)
  [PLAYER_LEFT, id] -> every player, the one leaving included (it gets it twice)
  removed from _players and _lastSeen (the id is free again)

next tick, game loop
  a PLAYER entity whose id is not connected any more -> destroyed`;

	const timeoutExample = `server   Network::_lastSeen[id] = now        on every packet of the player
         GameLoop, each tick: dropTimedOutPlayers()
           silent for more than PLAYER_TIMEOUT (2 s) -> "Player 0 timed out", disconnectPlayer

client   ReadSocket::_lastSnapshotReceived = now   on CONNECT_ACCEPT and on every SNAPSHOT
         after reading the socket, each frame:
           no snapshot for more than SNAPSHOT_TIMEOUT (2 s) -> the client stops`;

	const clientExample = `PLAYER_LEFT with my id      -> "Player eliminated by the server", the client stops
PLAYER_LEFT with another id -> nothing, the entity is gone from the next snapshot
no snapshot for 2 s         -> "Timeout: No snapshot received for 2 seconds, disconnecting"`;
</script>

<svelte:head>
	<title>How disconnects and timeouts work - R-Type docs</title>
</svelte:head>

{#snippet code(text)}
	<pre
		class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code
			>{text}</code
		></pre>
{/snippet}

{#snippet c(text)}<code
		class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]"
		>{text}</code
	>{/snippet}

<h1 class="text-3xl font-semibold">How disconnects and timeouts work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How a player leaves the game, and how both sides notice when the other one is gone
</p>

<p class="max-w-[100ch] leading-7">
	UDP has no connection: when a client crashes, the server receives nothing, not even an error. So
	both sides watch the packets of the other one and give up after 2 seconds of silence.
</p>
{@render code(waysExample)}

<h2 class="mt-9 text-lg font-semibold">Leaving the game</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Every way of leaving ends in {@render c('Network::disconnectPlayer')}, under
	{@render c('_playersMutex')}. It can be called from the network thread (a
	{@render c('DISCONNECT')}) or from the game thread (a timeout, a collision).
</p>
{@render code(leaveExample)}

<h2 class="mt-9 text-lg font-semibold">The timeouts</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The server checks the timeouts in the game thread, at each tick: the network thread would not see
	anything while it waits for a packet that never comes. The client checks after reading every
	packet waiting on the socket, so a client that froze for a moment reads its late snapshots first
	and is not stopped by mistake.
</p>
{@render code(timeoutExample)}

<h2 class="mt-9 text-lg font-semibold">On the client</h2>
{@render code(clientExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		A client that freezes for more than 2 seconds (a breakpoint, a window dragged on Windows) is
		dropped by the server. Raise {@render c('PLAYER_TIMEOUT')} if that is too strict.
	</li>
	<li>
		A player is only eliminated by an enemy. Bullets, stars and other players do not hurt it.
	</li>
	<li>
		The client sends {@render c('DISCONNECT')} only when it closes normally. If it is killed or
		crashes, the server notices with the timeout.
	</li>
	<li>
		An eliminated player that starts its client again from the same address and port is rejected
		with {@render c('SERVER_FULL')}. A new client process uses a new port, so it can join.
	</li>
</ul>
