<script>
	const flowExample = `client ──UDP──> network thread ──push──> InputQueue ──pop──> game loop (1 tick / s)`;

	const packetExample = `byte 0   message type   INPUT = 0x01
byte 1   pressed keys   UP = 0x01, DOWN = 0x02, LEFT = 0x04, RIGHT = 0x08, SHOOT = 0x10`;

	const runExample = `./r-type_server`;

	const sendExample = `# UP + RIGHT (0x01 | 0x08 = 0x09)
printf '\\x01\\x09' | nc -u -w1 127.0.0.1 1234`;

	const outputExample = `[tick] ----- nothing received
[tick] executed : UP RIGHT (9)`;
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
	What the game server receives and how to test it
</p>

<p class="max-w-[100ch] leading-7">
	{@render c('r-type_server')} receives player inputs over UDP on port {@render c('1234')} and
	handles them on a fixed tick. This is a first version that will change a lot, so this page only
	covers the basics.
</p>

<h2 class="mt-9 text-lg font-semibold">How it works</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A network thread receives the packets and stores the inputs in a queue. The game loop takes one
	input from the queue at each tick. For now it only prints it; the ECS will run there later.
</p>
{@render code(flowExample)}

<h2 class="mt-9 text-lg font-semibold">The INPUT packet</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The format is shared by the client and the server in {@render c('src/common/protocol.hpp')}. An
	input packet is 2 bytes, with one bit per key, so several keys can be sent at once:
</p>
{@render code(packetExample)}

<h2 class="mt-9 text-lg font-semibold">Testing it</h2>
<p class="mt-3 max-w-[100ch] leading-7">Start the server from the repository root:</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	In another terminal, send a packet with {@render c('nc')}:
</p>
{@render code(sendExample)}
<p class="mt-3 max-w-[100ch] leading-7">The server prints the keys it received at the next tick:</p>
{@render code(outputExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>The port is fixed to {@render c('1234')} for now.</li>
	<li>The server uses Linux sockets, so it does not build on Windows yet.</li>
</ul>
