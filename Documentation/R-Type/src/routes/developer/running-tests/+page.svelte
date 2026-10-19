<script>
	const triggerExample = `on:
  pull_request:
    branches: [main]`;

	const conditionExample = `jobs:
  tests:
    if: github.repository != 'EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4'
    runs-on: ubuntu-latest`;

	const jobsExample = `jobs:
  lint:
    # format check
  tests:
    # build, tests, clang-tidy`;

	const lintStepsExample = `- uses: actions/checkout@v4

- name: Install clang-format
  run: pip install clang-format==21.1.8

- name: Check formatting
  run: |
    clang-format --version
    shopt -s globstar nullglob
    files=(src/**/*.cpp src/**/*.hpp)
    if (( \${#files[@]} )); then
      clang-format --style=file:Norms/.clang-format --dry-run -Werror "\${files[@]}"
    fi`;

	const testsStepsExample = `- uses: actions/checkout@v4

- name: Install system libraries (OpenGL / X11 for raylib)
  run: sudo apt-get install -y --no-install-recommends libgl1-mesa-dev ... clang-tidy

- name: Restore CPM cache
  uses: actions/cache@v4

- name: Run tests
  run: ./tests/tests.sh

- name: clang-tidy (build with analysis)
  run: |
    cmake -B build-tidy -DENABLE_CLANG_TIDY=ON
    cmake --build build-tidy --parallel`;
</script>

<svelte:head>
	<title>Tests - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">Tests</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How the test script runs automatically on pull requests
</p>

<p class="max-w-[68ch] leading-7">
	The workflow {@render c('.github/workflows/tests.yml')} runs the project's checks when a pull
	request targeting <strong class="font-semibold">main</strong> is opened. It runs two jobs: a
	formatting job and a build/test job.
</p>

<h2 class="mt-9 text-lg font-semibold">When it runs</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	The workflow starts when a pull request targeting main is opened, and again each time new commits
	are pushed to it. It cannot be started manually.
</p>
{@render code(triggerExample)}

<h2 class="mt-9 text-lg font-semibold">Where it runs</h2>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Both jobs skip the Epitech repository, the copy that the mirror workflow pushes to, through the
		{@render c('if')} condition. They run on the team repository only.
	</li>
	<li>{@render c('contents: read')} is enough because the workflow only reads the code.</li>
	<li>Both jobs run on {@render c('ubuntu-latest')}.</li>
</ul>
{@render code(conditionExample)}

<h2 class="mt-9 text-lg font-semibold">Jobs</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	The {@render c('lint')} job installs a pinned clang-format (21.1.8) with pip and checks every C++
	source and header file under {@render c('src')} against {@render c('Norms/.clang-format')}. Any
	formatting difference fails the job. {@render c('shopt -s globstar')} is what makes
	{@render c('src/**')} search the sub-folders.
</p>
{@render code(lintStepsExample)}

<p class="mt-5 max-w-[68ch] leading-7">
	The {@render c('tests')} job installs raylib's Linux dependencies, restores the CPM cache, and
	runs {@render c('./tests/tests.sh')}. The script builds the project, then runs 109 tests against
	the real server: arguments, connection, snapshots, movement, shooting, timeouts and robustness (see
	<a href="/developer/tests-sh" class="text-[#ff8a3d] hover:underline">How tests.sh works</a>). The
	job finishes with a separate clang-tidy build in {@render c('build-tidy')}.
</p>
{@render code(testsStepsExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The workflow installs clang-format (pip), clang-tidy and the Linux libraries required by raylib
		(apt). CMake is not installed: it is already on the GitHub runner.
	</li>
	<li>
		{@render c('Norms/.clang-tidy')} has {@render c('WarningsAsErrors: \'\'')}: clang-tidy warnings
		are shown in the log but do not fail the job.
	</li>
	<li>
		The script must be executable ({@render c('chmod +x tests/tests.sh')}), or the step fails.
	</li>
	<li>
		The checks only run on pull requests targeting main, not on direct pushes or on a local merge
		pushed to main. Merge into main through a pull request so the checks run.
	</li>
</ul>