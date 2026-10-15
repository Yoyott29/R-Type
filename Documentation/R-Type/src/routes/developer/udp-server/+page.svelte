<script>
	const flowExample = `client ──UDP──> network thread ──push──> one InputQueue per player ──pop──> game loop (60 ticks / s)`;

	const connectExample = `client                                server
  [CONNECT, version] ──────────────────>  known version and a free slot?
                     <──────────────────  [CONNECT_ACCEPT, playerId]   (0 to 3)
                     <──────────────────  [CONNECT_REJECT, reason]     (otherwise)`;

	const packetExample = `CONNECT         0x10   [0x10, version]       client -> server
INPUT           0x30   [0x30, keys]          client -> server
CONNECT_ACCEPT  0x90   [0x90, playerId]      server -> client
CONNECT_REJECT  0x91   [0x91, reason]        server -> client

keys     UP = 0x01, DOWN = 0x02, LEFT = 0x04, RIGHT = 0x08, CHARGE = 0x10
reason   SERVER_FULL = 0x01, WRONG_VERSION = 0x02, NOT_CONNECTED = 0x04`;

	const runExample = `./r-type_server          # port 1234
./r-type_server 4242     # any port from 1 to 65535`;

	const sendExample = `# CONNECT with protocol version 1, prints the reply
printf '\\x10\\x01' | nc -u -w1 127.0.0.1 1234 | od -An -tx1
 90 00    → CONNECT_ACCEPT, you are player 0

# an INPUT from an address that never sent CONNECT
printf '\\x30\\x05' | nc -u -w1 127.0.0.1 1234 | od -An -tx1
 91 04    → CONNECT_REJECT, NOT_CONNECTED`;

	const outputExample = `Server listening on port : 1234
Player 0 connected: 127.0.0.1:51391
[tick 30] player 0 : UP LEFT (5)`;
</script>

<svelte:head>
	<title>How the server works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How the server works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How clients connect to the game server and how their inputs reach the game loop
</p>

<p class="max-w-[100ch] leading-7">
	{@render c('r-type_server')} talks to the clients over UDP with Asio, on port
	{@render c('1234')} by default. Up to 4 players can connect. This part will still change a lot,
	so this page only covers what works today.
</p>

<h2 class="mt-9 text-lg font-semibold">How it works</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A network thread receives every packet on a single socket and finds out which player sent it from
	its address. Each player has their own input queue, so inputs never get mixed. At each tick, the
	game loop takes one input from each player's queue. For now it only prints it; the ECS will run
	there later.
</p>
{@render code(flowExample)}

<h2 class="mt-9 text-lg font-semibold">Connecting</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A client must send {@render c('CONNECT')} before anything else. The server checks the protocol
	version, gives the client the smallest free player id and replies. Any other message from an
	address that is not connected is refused with {@render c('NOT_CONNECTED')}. If the client sends
	{@render c('CONNECT')} again because the reply got lost, it gets the same id back.
</p>
{@render code(connectExample)}

<h2 class="mt-9 text-lg font-semibold">The packets</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The format is shared by the client and the server in {@render c('src/common/protocol.hpp')}. The
	first byte is the message type: bit 7 is the direction (0 from the client, 1 from the server).
	These messages are handled today:
</p>
{@render code(packetExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The file also defines the next messages ({@render c('DISCONNECT')}, {@render c('SNAPSHOT')},
	{@render c('DELTA')}...), which the server does not handle yet.
</p>

<h2 class="mt-9 text-lg font-semibold">Testing it</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Start the server from the repository root, with an optional port:
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	In another terminal, send packets with {@render c('nc')} and read the reply with
	{@render c('od')}. Each {@render c('nc')} uses a new port, so the server sees a new client every
	time:
</p>
{@render code(sendExample)}
<p class="mt-3 max-w-[100ch] leading-7">The server logs each connection and each input it runs:</p>
{@render code(outputExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		An {@render c('INPUT')} is still 2 bytes; the tick number and the snapshot acknowledgement
		written in {@render c('protocol.hpp')} are not sent yet.
	</li>
	<li>Players are never removed yet: there is no disconnect or timeout.</li>
	<li>
		Sending the text {@render c('exit')} stops the server. It is a debug shortcut that will be
		removed.
	</li>
</ul>
