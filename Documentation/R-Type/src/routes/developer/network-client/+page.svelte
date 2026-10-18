<script>
	const frameExample = `each frame of Application::run
  PrepUpdate        GetInput (keys)        TryConnect (until connected)
  StateTransition   SendInput (INPUT)      ReadSocket (packets, newest snapshot)
  FixedUpdate       physics and scripts, 0 to n times
  PostUpdate        RenderSystem (draws the entities)`;

	const socketExample = `struct Socket {
    asio::ip::udp::socket socket;     // connected to the server, non-blocking
    std::string serverAddress;        // from the command line
    uint16_t serverPort;
    bool connected;                   // set by CONNECT_ACCEPT
    uint8_t playerId;
    uint32_t tick;                    // tick of the next INPUT
    uint32_t lastSnapshotTick;        // newest snapshot applied
    std::chrono::steady_clock::time_point lastConnectAttempt;
};`;

	const runExample = `./r-type_client                     # 127.0.0.1:4242
./r-type_client 5000                # 127.0.0.1:5000
./r-type_client 82.64.10.20 4444    # a remote server`;
</script>

<svelte:head>
	<title>How the client network works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How the client network works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	The Client plugin: the socket resource and the three systems that talk to the server
</p>

<p class="max-w-[100ch] leading-7">
	The client network lives in {@render c('src/plugins/Network/Client')}. The {@render c('Client')}
	plugin adds a {@render c('Socket')} resource and three systems to the scene. Everything runs in
	the main thread: the socket is non-blocking, so reading it never stops the frame.
</p>
{@render code(frameExample)}

<h2 class="mt-9 text-lg font-semibold">The Socket resource</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The UDP socket is {@render c('connect')}ed to the server, so it only receives the packets of that
	server. The resource also keeps the connection state shared by the three systems.
</p>
{@render code(socketExample)}

<h2 class="mt-9 text-lg font-semibold">The three systems</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('TryConnect')} opens the socket and sends {@render c('CONNECT')} every second until
		the client is accepted, then removes itself. See
		<a href="/developer/network-connection" class="text-[#ff8a3d] hover:underline">How connecting works</a>.
	</li>
	<li>
		{@render c('SendInput')} sends one {@render c('INPUT')} per frame once connected. See
		<a href="/developer/network-inputs" class="text-[#ff8a3d] hover:underline">How inputs work</a>.
	</li>
	<li>
		{@render c('ReadSocket')} reads every packet waiting, applies the newest snapshot and checks the
		snapshot timeout. See
		<a href="/developer/network-snapshots" class="text-[#ff8a3d] hover:underline">How snapshots work</a>
		and
		<a href="/developer/network-disconnect" class="text-[#ff8a3d] hover:underline">How disconnects and timeouts work</a>.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Starting and stopping</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The client takes an optional ip and port, checked before the window opens:
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The client stops when its window is closed, when it is rejected or eliminated, or when the server
	is silent for 2 seconds. In every case {@render c('Client::cleanup')} runs: it sends
	{@render c('DISCONNECT')} if the client was connected and closes the socket.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The client has no game logic of its own: everything it draws comes from the snapshots.
	</li>
	<li>
		Without a screen, the window cannot open: the client prints
		{@render c('Fatal error: Could not open the window')} and exits with 84.
	</li>
	<li>
		The messages printed with {@render c('std::cout')} only show up when the client exits if its
		output is redirected to a file. Errors use {@render c('std::cerr')} and show up right away.
	</li>
</ul>
