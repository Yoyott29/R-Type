<script>
	const runExample = `./tests/tests.sh`;

	const funcExample = `run_test() {
    local name="$1"; shift
    if "$@" > /tmp/test_output.log 2>&1; then
        echo "  [PASS] $name"
        passed=$((passed + 1))
    else
        echo "  [FAIL] $name"
        sed 's/^/         /' /tmp/test_output.log | tail -n 20
        failed=$((failed + 1))
        failed_names+=("$name")
    fi
}`;

	const buildExample = `echo "== Build =="
run_test "build" bash -c "cmake -B build && cmake --build build"`;

	const testsExample = `echo "== Tests =="
run_test "client binary exists"  test -x ./r-type_client`;

	const outputExample = `== Build ==
  [PASS] build
== Tests ==
  [PASS] client binary exists

================================
 Passed: 2
 Failed: 0
 Total:  2
================================`;

	const addExample = `run_test "client binary exists"  test -x ./r-type_client
run_test "my new test"           ./build/my_test_binary
run_test "output is correct"     bash -c "./r-type_client --version | grep -q 1.0"`;
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
	your machine, and the CI runs the same script on every pull request.
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Run it <strong class="font-semibold">from the repository root</strong>. The paths in the script, such
	as {@render c('./r-type_client')} and the {@render c('build')} folder, are relative to the root.
</p>

<h2 class="mt-9 text-lg font-semibold">The run_test function</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Every check goes through {@render c('run_test')}. It takes a name and a command. The command output
	is written to {@render c('/tmp/test_output.log')}. If the command succeeds, the test is
	<strong class="font-semibold">PASS</strong> and a counter goes up. If it fails, the test is
	<strong class="font-semibold">FAIL</strong>, the last 20 lines of its output are shown, and its name
	is remembered for the final report.
</p>
{@render code(funcExample)}

<h2 class="mt-9 text-lg font-semibold">The build step</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The first test is the build itself. If the project does not compile, you see the compiler and
	clang-tidy output right away.
</p>
{@render code(buildExample)}

<h2 class="mt-9 text-lg font-semibold">The tests</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Tests are plain commands: a test passes when its command exits with code 0.
</p>
{@render code(testsExample)}

<h2 class="mt-9 text-lg font-semibold">The summary and exit code</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	At the end, the script prints the number of passed and failed tests, and lists the failed ones. It
	exits with {@render c('1')} when at least one test failed. A non-zero exit code is what makes the CI
	job fail, and a failing pull request check.
</p>
{@render code(outputExample)}

<h2 class="mt-9 text-lg font-semibold">Adding a test</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Add one {@render c('run_test')} line in the Tests section, with a name and any command that returns 0
	on success:
</p>
{@render code(addExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The script already contains a commented line for a future server binary check, to enable once the
	server target exists.
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
		The workflow that calls it is described in
		<a href="/developer/running-tests" class="text-[#ff8a3d] hover:underline">Running Tests</a>.
	</li>
</ul>