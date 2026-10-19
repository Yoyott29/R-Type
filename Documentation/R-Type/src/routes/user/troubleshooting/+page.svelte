<svelte:head><title>Troubleshooting - R-Type docs</title></svelte:head>
<h1 class="text-3xl font-semibold">Troubleshooting</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Common launch and gameplay problems</p>

<h2 class="mt-9 text-lg font-semibold">The client keeps printing "Connecting to ..."</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The client gets no answer and tries again every second, forever. Check that:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>the server is running;</li>
	<li>the client uses the same address and port as the server;</li>
	<li>for another machine, the server was started with <code>0.0.0.0</code> and the firewall allows UDP on that port.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Messages printed by the client</h2>
<div class="mt-3 max-w-[100ch] overflow-hidden rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Message</th><th class="px-4 py-3">What it means</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Connection rejected by the server: server full</td><td class="px-4 py-3">The four slots are taken, or this address already left or was eliminated. Start the client again.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Connection rejected by the server: wrong protocol version</td><td class="px-4 py-3">The client and the server were built from different versions. Rebuild both from the same commit.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Connection rejected by the server: unknown reason</td><td class="px-4 py-3">The server no longer knows this client (it timed out). Start the client again.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Player eliminated by the server</td><td class="px-4 py-3">Your ship touched an enemy. Start the client again to rejoin.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Timeout: No snapshot received for 2 seconds, disconnecting</td><td class="px-4 py-3">The server stopped, or the network dropped the packets for 2 seconds.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Fatal error: File not found: Assets/...png</td><td class="px-4 py-3">A texture is missing. Run the client from the repository root.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Fatal error: Unknown component type: ...</td><td class="px-4 py-3">The client received a snapshot it cannot read, usually from a server of another version. Rebuild both from the same commit.</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Fatal error: Could not open the window</td><td class="px-4 py-3">No display is available (SSH session, missing graphics drivers).</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 font-mono">Invalid ip / Invalid port</td><td class="px-4 py-3">The arguments are wrong. Use <code>./r-type_client [ip] [port]</code> with an IPv4 address.</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Address already in use</h2>
<p class="mt-3 max-w-[100ch] leading-7">Another server is listening on UDP port 4242. Stop it, or start the server on another port and give the same port to the clients: <code>./r-type_server 5000</code> and <code>./r-type_client 5000</code>.</p>

<h2 class="mt-9 text-lg font-semibold">Build failures</h2>
<pre class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>cmake -B build
cmake --build build --target r-type_client r-type_server -j2</code></pre>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>CMake downloads raylib and Asio on the first configure: it needs an internet connection.</li>
	<li>On Linux, install the OpenGL and X11 development packages listed in <a href="/user/getting-started" class="text-[#ff8a3d] hover:underline">Getting started</a>.</li>
	<li>The warning about clang-tidy not being found is harmless; configure with <code>-DENABLE_CLANG_TIDY=OFF</code> to hide it.</li>
</ul>
