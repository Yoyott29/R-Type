<script>
	const runExample = `./tests/tests.sh`;

	const funcExample = `run_test() {
    local name="$1"; shift
    local status

    test_index=$((test_index + 1))
    PORT=$((PORT_BASE + test_index))
    ( trap cleanup EXIT; "$@" ) > /tmp/test_output.log 2>&1
    status=$?
    if [ "$status" -eq 0 ]; then
        echo "  [PASS] $name"
        passed=$((passed + 1))
    elif [ "$status" -eq 77 ]; then
        echo "  [SKIP] $name ($(tail -n 1 /tmp/test_output.log))"
        skipped=$((skipped + 1))
    else
        echo "  [FAIL] $name"
        sed 's/^/         /' /tmp/test_output.log | tail -n 20
        failed=$((failed + 1))
        failed_names+=("$name")
    fi
}`;

	const buildExample = `echo "== Build =="
run_test "build" bash -c "cmake -B build && cmake --build build"`;

	const helpersExample = `fail() { echo "FAIL: $*"; exit 1; }
skip() { echo "$*"; exit 77; }
check() { awk "BEGIN { exit !($1) }" || fail "\${2:-expected $1}"; }

# Opens a new UDP socket to the server in the variable named $1 (a new client, a new source port)
open_client() { eval "exec {$1}<>/dev/udp/$LOCAL/$PORT"; }`;

	const testsExample = `# Holding RIGHT moves the player to the right
test_move_right() {
    start_server
    connected_clients A
    hold "$A" "$RIGHT" 0.4 > /dev/null
    check "$(player_x "$(last_snapshot "$A")") > 150" "the player did not move right"
}

echo "== Movement =="
run_test "move right" test_move_right`;

	const outputExample = `== Build ==
  [PASS] build
== Binaries ==
  [PASS] server binary exists
  [PASS] client binary exists
== Server arguments ==
  [PASS] server: port not a number
  ...
== Real client ==
  [SKIP] client connects (no display to open the client window)
  ...

================================
 Passed:  103
 Failed:  0
 Skipped: 6
 Total:   109
================================`;

	const addExample = `# A client with the protocol version 3 is rejected with WRONG_VERSION
test_connect_version_three() {
    start_server
    open_client A
    [ "$(connect_client "$A" 03)" = "REJECT 2" ] || fail "version 3 not rejected"
}

# in the Run part, under echo "== Connection =="
run_test "version 3 rejected" test_connect_version_three`;
</script>

<svelte:head>
	<title>How tests.sh works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How tests.sh works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	What the project test script does, step by step
</p>

<p class="max-w-[100ch] leading-7">
	{@render c('tests/tests.sh')} is the single entry point to check the project: it builds the code,
	runs every test, prints a summary and returns an error code if something failed. You can run it on
	your machine, and the CI runs the same script on every pull request that targets
	{@render c('main')}.
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	You can call it from any folder: the script starts with {@render c('cd "$(dirname "$0")/.."')}, so
	the paths it uses, such as {@render c('./r-type_server')}, {@render c('./r-type_client')} and the
	{@render c('build')} folder, are always relative to the repository root.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	The tests only use the compiled binaries: they start the server (and sometimes the real client),
	talk to it over raw UDP with bash's {@render c('/dev/udp')}, and check what comes back. No test
	framework is needed.
</p>

<h2 class="mt-9 text-lg font-semibold">The run_test function</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Every check goes through {@render c('run_test')}. It takes a name and a command (usually a
	{@render c('test_*')} function). Each test gets its own UDP port: {@render c('PORT')} is
	{@render c('PORT_BASE')} (derived from the script PID) plus the test index, so tests never reuse a
	port. The command runs in a subshell whose {@render c('cleanup')} trap kills the server, the client
	and any background job when it ends. Its output is written to
	{@render c('/tmp/test_output.log')}.
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Exit code {@render c('0')}: the test is <strong class="font-semibold">PASS</strong>.</li>
	<li>
		Exit code {@render c('77')}: the test is <strong class="font-semibold">SKIP</strong>, and the
		last line of its output is shown as the reason.
	</li>
	<li>
		Anything else: the test is <strong class="font-semibold">FAIL</strong>, the last 20 lines of its
		output are shown, and its name is remembered for the final report.
	</li>
</ul>
{@render code(funcExample)}

<h2 class="mt-9 text-lg font-semibold">The build step</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The first test is the build itself. If the project does not compile, you see the compiler and
	clang-tidy output right away ({@render c('ENABLE_CLANG_TIDY')} is {@render c('ON')} by default and
	runs on the client sources when clang-tidy is installed). The binaries are written to the
	repository root.
</p>
{@render code(buildExample)}

<h2 class="mt-9 text-lg font-semibold">The helpers</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Before the tests, the script defines small helpers so each test stays a few lines long:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('fail')}, {@render c('skip')} and {@render c('check')}: stop the test with exit 1, exit
		77, or fail when an {@render c('awk')} condition is false.
	</li>
	<li>
		{@render c('start_server')}, {@render c('stop_server')}, {@render c('start_client')}: start or stop
		the real binaries on {@render c('127.0.0.1')} and the test's port, with logs in temporary files.
		{@render c('run_binary')} runs a binary that should exit by itself (5 s timeout) and keeps its
		output in {@render c('$RUN_LOG')}.
	</li>
	<li>
		{@render c('open_client')}, {@render c('send_bytes')}, {@render c('send_input')},
		{@render c('disconnect_client')}, {@render c('connect_client')},
		{@render c('connected_clients')}: fake clients that open a UDP socket and send raw packets.
	</li>
	<li>
		{@render c('listen')}, {@render c('hold')}, {@render c('last_snapshot')},
		{@render c('count_type')}, {@render c('player_x')}, {@render c('player_y')}: decode the server
		packets (ACCEPT, REJECT, LEFT, STARTED, SNAPSHOT) into one text line each and read them.
	</li>
	<li>
		{@render c('wait_log')}, {@render c('wait_exit')}, {@render c('is_running')}: wait for a log line
		or a process exit with a timeout.
	</li>
	<li>
		{@render c('need_display')}: skips the test when neither {@render c('DISPLAY')} nor
		{@render c('WAYLAND_DISPLAY')} is set.
	</li>
</ul>
{@render code(helpersExample)}

<h2 class="mt-9 text-lg font-semibold">The tests</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each test is a {@render c('test_*')} function: it passes when it returns 0. At the end of the file,
	the Run part calls every function with {@render c('run_test')}, grouped in sections:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('Build')}: the CMake build.</li>
	<li>{@render c('Binaries')}: {@render c('r-type_server')} and {@render c('r-type_client')} exist and are executable.</li>
	<li>
		{@render c('Server arguments')} and {@render c('Client arguments')}: invalid ports, ips or too many
		arguments must exit with code {@render c('84')}; default address and port of the server.
	</li>
	<li>{@render c('Server start and shutdown')}: logs, SIGINT / SIGTERM exit with 0, port freed.</li>
	<li>{@render c('Connection')}: player ids, server full, wrong version, packets before CONNECT.</li>
	<li>{@render c('Snapshots')}: content, spawn points, about 60 per second, who receives them.</li>
	<li>{@render c('Movement')}: directions, speed, screen edges, wrong size INPUT.</li>
	<li>{@render c('Shooting')}: bullets, cooldown, bullets leaving the screen.</li>
	<li>{@render c('Disconnect and timeout')}: PLAYER_LEFT, freed slots, 2 s timeout.</li>
	<li>{@render c('Game')}: players overlapping, enemies and waves.</li>
	<li>{@render c('Robustness')}: garbage, huge packets, floods, vanished clients.</li>
	<li>
		{@render c('Real client')}: the real client against the server; these tests need a screen and are
		skipped otherwise.
	</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	There are 109 {@render c('run_test')} calls in total: the build and 108 tests. The packet formats
	the tests check are described in
	<a href="/developer/network-protocol" class="text-[#ff8a3d] hover:underline">Network Protocol</a>.
</p>
{@render code(testsExample)}

<h2 class="mt-9 text-lg font-semibold">The summary and exit code</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	At the end, the script prints the number of passed, failed and skipped tests, and lists the failed
	ones. It exits with {@render c('1')} when at least one test failed; skipped tests do not make it
	fail. A non-zero exit code is what makes the CI job fail, and a failing pull request check. Sample
	output on a machine without a screen (trimmed):
</p>
{@render code(outputExample)}

<h2 class="mt-9 text-lg font-semibold">Adding a test</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Write a {@render c('test_*')} function in the matching section of the file, using the helpers and
	{@render c('fail')} / {@render c('check')} to report errors, then add one {@render c('run_test')}
	line under the right {@render c('echo "== ... =="')} in the Run part. You do not need to pick a
	port or stop the server yourself: {@render c('run_test')} handles both. Example (illustrative):
</p>
{@render code(addExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	If the test cannot run in some environments, call {@render c('skip "reason"')} (or
	{@render c('need_display')}) instead of failing.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The script must be executable: {@render c('chmod +x tests/tests.sh')}.
	</li>
	<li>
		The script rebuilds the project in {@render c('build')}, so it also checks that CMake and
		dependencies work from the current state of the repository.
	</li>
	<li>
		The server tests bind ports above 20000 on {@render c('127.0.0.1')}; one test also checks the
		default port 4242 and is skipped if it is already in use.
	</li>
	<li>
		The workflow that calls it is described in
		<a href="/developer/running-tests" class="text-[#ff8a3d] hover:underline">Running Tests</a>.
	</li>
</ul>
