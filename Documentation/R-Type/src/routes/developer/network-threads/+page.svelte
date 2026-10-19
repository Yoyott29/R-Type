<script>
	const threadsExample = `main thread = game thread                     network thread
GameLoop::run()                                Network::receiveLoop()
  60 ticks per second, never waits on clients    blocks on receive_from()
  owns the Scene                                 CONNECT / INPUT / DISCONNECT
  sends the snapshots                            never touches the Scene`;

	const sharedExample = `shared data                     protected by
InputQueue of each player       its own mutex (push: network thread, pop: game thread)
_players, _eliminatedPlayers,   _playersMutex
_lastSeen
_running                        std::atomic<bool>`;

	const stopExample = `Ctrl+C (SIGINT) or SIGTERM
  -> requestStop() sets stopRequested           (signal handler, only an atomic)
  -> the game loop ends its tick and leaves
  -> Network::stop(): _running = false + a 1 byte datagram to its own port
  -> receive_from() returns, the network thread leaves
  -> Server::run() joins the thread, the socket is closed: "Closing server"`;

	const errorsExample = `exception in the game loop     -> the network thread is stopped and joined,
                                  then main prints "Fatal error: ..." and exits with 84
exception while reading a packet -> "Dropped packet: ...", the packet is ignored
connection_reset / refused      -> ignored (a client closed on Windows)
any other socket error          -> "Network: ...", the network thread stops and sets
                                  _running to false, so the game loop ends too:
                                  the whole server stops`;
</script>

<svelte:head>
	<title>How the server threads work - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How the server threads work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	Why the server has two threads, what they share and how they stop
</p>

<p class="max-w-[100ch] leading-7">
	The game must run frame after frame whatever the clients do, so the server never waits for a
	packet in the game loop. {@render c('Server::run')} starts a network thread that only receives,
	and runs the game loop on the main thread.
</p>
{@render code(threadsExample)}

<h2 class="mt-9 text-lg font-semibold">What they share</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The rule is simple: only the game thread touches the {@render c('Scene')}. The network thread
	does not create entities or read components, it drops messages in protected places, and the game
	thread picks them up at the start of its next tick.
</p>
{@render code(sharedExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The map of input queues is filled for the 4 player ids in the {@render c('Server')} constructor
	and never changes shape after that, so both threads can look it up without a lock.
</p>

<h2 class="mt-9 text-lg font-semibold">Stopping</h2>
{@render code(stopExample)}

<h2 class="mt-9 text-lg font-semibold">Errors</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A thread that is still running when an exception leaves {@render c('main')} would abort the whole
	process, so every error is caught where it happens:
</p>
{@render code(errorsExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The game thread also sends on the socket (snapshots, {@render c('PLAYER_LEFT')}) while the
		network thread receives on it. Asio does not guarantee that one socket can be used from two
		threads at once. It works on Linux, but a send queue read by the network thread would be safe.
	</li>
	<li>
		The server does not read stdin, so it can run under nohup, docker or CI. Stop it with Ctrl+C
		or {@render c('kill')}.
	</li>
	<li>
		Several games in parallel would need one game thread and one {@render c('Scene')} per game,
		with the network thread sending each packet to the right game.
	</li>
</ul>
