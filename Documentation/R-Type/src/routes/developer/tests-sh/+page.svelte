<script>
	const code0 = `./tests/tests.sh`;
	const code1 = `# A port that is not a number is refused with exit code 84 and an error message
test_server_args_port_not_a_number() {
    run_binary "$SERVER_BIN" abc; [ $? -eq 84 ] || fail "exit code is not 84"
    grep -q "Invalid port" <<< "$RUN_LOG" || fail "no 'Invalid port' message"
}

run_test "server: port not a number" test_server_args_port_not_a_number`;
</script>

<svelte:head>
	<title>How tests.sh works - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">How tests.sh works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">The end-to-end tests: real server, real client, checked from the outside</p>

<p class="mt-3 max-w-[100ch] leading-7">
	<code>tests/tests.sh</code> builds the project, runs every test, prints a summary and exits with 1 if a test failed. It only uses the compiled programs: it starts <code>r-type_server</code> and <code>r-type_client</code> and checks their exit codes, logs and sockets. Linux only.
</p>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code0}</code></pre>

<h2 class="mt-9 text-lg font-semibold">The run_test function</h2>

<p class="mt-3 max-w-[100ch] leading-7">
	Every test goes through <code>run_test name command</code>. Each test gets its own UDP port (<code>PORT_BASE</code>, taken from the script's process id, plus the test number), runs in a subshell that kills its processes on exit, and ends in one of three ways:
</p>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Exit code</th><th class="px-4 py-3">Result</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">0</td><td class="px-4 py-3 align-top"><code>[PASS]</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">77</td><td class="px-4 py-3 align-top"><code>[SKIP]</code>, with the reason (for example no display)</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">anything else</td><td class="px-4 py-3 align-top"><code>[FAIL]</code>, with the last 20 lines of the test's output</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">The helpers</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Helper</th><th class="px-4 py-3">What it does</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>fail</code>, <code>skip</code></td><td class="px-4 py-3 align-top">End the test as failed (exit 1) or skipped (exit 77), with a message</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>start_server</code>, <code>stop_server</code></td><td class="px-4 py-3 align-top">Start the server on <code>127.0.0.1:$PORT</code> and wait until its socket is bound; stop it with a signal (SIGINT by default) and wait for it</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>start_client</code></td><td class="px-4 py-3 align-top">Start the real client on the same address</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>run_binary</code></td><td class="px-4 py-3 align-top">Run a program that should exit by itself (5 s at most), keeping its output in <code>$RUN_LOG</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>wait_log</code>, <code>wait_bound</code>, <code>wait_exit</code>, <code>is_running</code></td><td class="px-4 py-3 align-top">Wait for a line in a log, a bound UDP socket or a process exit, with a timeout</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top"><code>need_display</code></td><td class="px-4 py-3 align-top">Skip the test when there is no display to open the client window</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">The tests</h2>

<div class="mt-3 max-w-[100ch] overflow-x-auto rounded-md border border-[#1e2b26]">
	<table class="w-full text-left text-[15px]">
		<thead class="bg-[#121a17]"><tr><th class="px-4 py-3">Section</th><th class="px-4 py-3">Tests</th><th class="px-4 py-3">What they check</th></tr></thead>
		<tbody>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Build</td><td class="px-4 py-3 align-top">1</td><td class="px-4 py-3 align-top"><code>cmake -B build &amp;&amp; cmake --build build</code> succeeds</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Binaries</td><td class="px-4 py-3 align-top">2</td><td class="px-4 py-3 align-top">Both programs exist and can be executed</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Server arguments</td><td class="px-4 py-3 align-top">13</td><td class="px-4 py-3 align-top">Bad ports, bad addresses and too many arguments exit with code 84; an unknown address or a used port fail; the default address and port</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Client arguments</td><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">Bad ports exit with 84; without a display, the client exits with 84 and <code>Could not open the window</code></td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Server start and shutdown</td><td class="px-4 py-3 align-top">5</td><td class="px-4 py-3 align-top">Runs with a closed stdin, exits with 0 on SIGINT and SIGTERM in less than a second, frees its port</td></tr>
			<tr class="border-t border-[#1e2b26]"><td class="px-4 py-3 align-top">Real client</td><td class="px-4 py-3 align-top">3</td><td class="px-4 py-3 align-top">The client connects (<code>Player 1 connected</code>), stays alive, and a killed client is dropped by the server</td></tr>
		</tbody>
	</table>
</div>

<h2 class="mt-9 text-lg font-semibold">Adding a test</h2>

<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-6 leading-7">
	<li>Write a <code>test_...</code> function in the tests part of the script, using the helpers. End with <code>fail</code> when something is wrong.</li>
	<li>Call it with <code>run_test "short name" test_...</code> in the right section of the run part.</li>
	<li>If it needs the client window, start it with <code>need_display</code>.</li>
</ol>

<pre
	class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code>{code1}</code></pre>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>

<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-6 leading-7">
	<li>The real client tests need a display: they are skipped in a terminal without one.</li>
	<li>Every process a test starts is killed when the test ends, even when it fails.</li>
	<li>The engine has its own unit tests, run by CTest: see <a href="/developer/running-tests" class="text-accent hover:underline">Running Tests</a>.</li>
</ul>
