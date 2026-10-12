<script>
	const packetExample = `byte 0   message type   INPUT = 0x01
byte 1   pressed keys   UP = 0x01, DOWN = 0x02, LEFT = 0x04, RIGHT = 0x08, SHOOT = 0x10`;

	const testExample = `./r-type_server

# in another terminal: send UP + RIGHT (0x01 | 0x08 = 0x09)
printf '\\x01\\x09' | nc -u -w1 127.0.0.1 1234`;
</script>

<svelte:head>
	<title>UDP Server - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">UDP Server</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">The game server and the packets it receives</p>

<p class="max-w-[100ch] leading-7">
	{@render c('r-type_server')} receives player inputs over UDP on port {@render c('1234')}. This is
	a first version and it will change a lot, so this page only gives the basics.
</p>

<h2 class="mt-9 text-lg font-semibold">Packet format</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The format is shared by the client and the server in {@render c('src/common/protocol.hpp')}. An
	input packet is 2 bytes, with one bit per key, so several keys can be sent at once:
</p>
{@render code(packetExample)}

<h2 class="mt-9 text-lg font-semibold">Testing</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Start the server, then send a packet with {@render c('nc')}. The server prints the keys it
	received at the next tick.
</p>
{@render code(testExample)}
