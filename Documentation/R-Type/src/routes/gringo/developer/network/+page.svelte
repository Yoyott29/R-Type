<script>
	const buildExample = `"Gringo Engine/Gringo" --build R-Type/R-Type.gringo
  -> R-Type/r-type_server    headless: no renderer, renderer plugins skipped
  -> R-Type/r-type_client    renderer + every plugin`;

	const systemsExample = `            server                                   client
PrepUpdate  events, NetReceive, SideFilter,          events, NetReceive, SideFilter,
            Player, RemoteInput                      Mirror
PostUpdate  Replication, NetSend                     InputSend, NetSend`;
</script>

<svelte:head>
	<title>How the network works - Gringo docs</title>
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

{#snippet a(href, text)}<a {href} class="text-accent hover:underline">{text}</a>{/snippet}

<h1 class="text-3xl font-semibold">How the network works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	The Network plugin: one project, a server and a client build
</p>

<p class="max-w-[100ch] leading-7">
	Networking is a plugin ({@render c('Gringo Engine/src/plugins/Network')}). The engine, the runtime
	and the editor know nothing about it. Any {@render c('.gringo')} project that activates
	{@render c('Network')} is built as two executables: an authoritative, headless
	<strong>server</strong> that runs the game, and <strong>clients</strong> that show mirrors of its
	entities and send their input back, over UDP.
</p>

<h2 class="mt-9 text-lg font-semibold">Build and run</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A plugin declares build variants ({@render c('IPlugin::buildVariants')}); the builder emits one
	executable per variant and the running one reads its name from the {@render c('BuildVariantName')}
	resource. A project without Network still builds one executable. Running them:
	{@render a('/user/multiplayer', 'Multiplayer')}.
</p>
{@render code(buildExample)}

<h2 class="mt-9 text-lg font-semibold">Settings and the Networked component</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Project settings: {@render c('Host')} (IPv4, default 127.0.0.1), {@render c('Port')} (default
		4242), {@render c('PlayerPrefabs')} (comma-separated, see
		{@render a('/gringo/developer/network-players', 'How players and inputs work')}).
	</li>
	<li>
		{@render c('Networked.Mode')}: {@render c('Replicated')} (server keeps it, clients get a mirror),
		{@render c('Server')} or {@render c('Client')} (only that side keeps it). An entity without
		{@render c('Networked')} exists on both sides and is not synced.
	</li>
	<li>
		{@render c('Networked.Prefab')}: what a client instantiates as the mirror. It must name an existing
		prefab.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Systems</h2>
{@render code(systemsExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('SideFilter')} destroys the entities whose {@render c('Mode')} is not for this side;
	{@render c('Replication')} and {@render c('Mirror')} are in
	{@render a('/gringo/developer/network-replication', 'How replication works')}. The {@render c('UdpSocket')} resource
	owns the asio socket on its own I/O thread (receive queue capped at 1024 datagrams). The receive,
	send and input send systems all write {@render c('NetTransport')}, so the scheduler runs them one
	at a time and the socket needs no other lock.
</p>

<h2 class="mt-9 text-lg font-semibold">Errors</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Bad network data is dropped and logged once per type; it never stops a running game. Startup
	errors (bad IP or port, port in use, unknown player prefab) stop the executable. Game messages and
	the wire format: {@render a('/gringo/developer/network-protocol', 'How the protocol works')}.
</p>
