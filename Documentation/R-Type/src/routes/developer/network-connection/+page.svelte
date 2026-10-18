<script>
	const handshakeExample = `client (TryConnect)                     server (Network::handleConnect)
  [CONNECT, 1] ───────────────────────>  size is 2? version is 1?
                                         address not eliminated? a free id?
               <───────────────────────  [CONNECT_ACCEPT, playerId]   (0 to 3)
               <───────────────────────  [CONNECT_REJECT, reason]     (otherwise)
  no answer after 1 s: CONNECT again`;

	const checksExample = `CONNECT is not 2 bytes            -> no answer
version is not PROTOCOL_VERSION   -> CONNECT_REJECT WRONG_VERSION
address left or was eliminated    -> CONNECT_REJECT SERVER_FULL
address already connected         -> CONNECT_ACCEPT with the same id
a free id (0 to 3)                -> CONNECT_ACCEPT with the smallest free id
4 players already                 -> CONNECT_REJECT SERVER_FULL
any other packet, unknown address -> CONNECT_REJECT NOT_CONNECTED`;

	const spawnExample = `player 0 -> (100, 200)
player 1 -> (100, 300)
player 2 -> (100, 400)
player 3 -> (100, 500)`;

	const logExample = `server   Player 0 connected: 127.0.0.1:51391
client   Connecting to 127.0.0.1:4242
client   Connected to 127.0.0.1:4242 as player 0
client   Connection rejected by the server: server full`;
</script>

<svelte:head>
	<title>How connecting works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How connecting works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How a client joins the game and gets its player id
</p>

<p class="max-w-[100ch] leading-7">
	A client must send {@render c('CONNECT')} before anything else. The server knows the players by
	their address (ip and port): a new address is a new client. Up to 4 players can be connected at
	the same time, with the ids {@render c('0')} to {@render c('3')}.
</p>

<h2 class="mt-9 text-lg font-semibold">The handshake</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The client system {@render c('TryConnect')} opens the socket and sends {@render c('CONNECT')}
	with the protocol version. UDP can lose packets, so it sends it again every second
	({@render c('CONNECT_RETRY_DELAY')}) until the server answers. Once accepted, it removes itself
	from the scene.
</p>
{@render code(handshakeExample)}

<h2 class="mt-9 text-lg font-semibold">What the server checks</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('Network::handleConnect')} runs these checks in order, under
	{@render c('_playersMutex')}. A client that sends {@render c('CONNECT')} again because the answer
	got lost gets the same id back.
</p>
{@render code(checksExample)}

<h2 class="mt-9 text-lg font-semibold">The player entity</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	On the next tick, the game loop sees the new id in {@render c('getConnectedPlayerIds')} and
	creates the player entity: {@render c('Type PLAYER')}, {@render c('PlayerId')},
	{@render c('Position')}, {@render c('Velocity')}, {@render c('Sprite')},
	{@render c('Collider2D')} and a script that keeps it on screen. Each player starts 100 pixels
	below the previous one:
</p>
{@render code(spawnExample)}

<h2 class="mt-9 text-lg font-semibold">On the client</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('ReadSocket')} handles the answer. {@render c('CONNECT_ACCEPT')} stores the player id
	in the {@render c('Socket')} resource and sets {@render c('connected')}, which starts the inputs
	and the snapshot timeout. {@render c('CONNECT_REJECT')} prints the reason and stops the client.
	Both sides log the connection:
</p>
{@render code(logExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		An address that left or was eliminated stays in {@render c('_eliminatedPlayers')} and cannot
		come back. A restarted client uses a new port, so it is a new address and can join again.
	</li>
	<li>
		The freed ids are given again, the smallest first: if player 1 leaves, the next client becomes
		player 1.
	</li>
	<li>
		There is no lobby yet: a player joins the game that is already running.
	</li>
</ul>
