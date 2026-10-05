<script>
	const triggerExample = `on:
  pull_request:
    branches: [main]`;

	const conditionExample = `jobs:
  tests:
    if: >
      github.repository != 'EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4' &&
      github.head_ref == 'dev'
    runs-on: ubuntu-latest`;

	const stepsExample = `- uses: actions/checkout@v4

- name: Run tests
  run: ./tests/tests.sh`;
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
	The workflow {@render c('.github/workflows/tests.yml')} runs the project's tests when a pull
	request from <strong class="font-semibold">dev</strong> is opened against
	<strong class="font-semibold">main</strong>. The result appears as a check on the pull request.
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
		The first {@render c('if')} condition skips the job on the original school repository, so only
		other copies run the tests.
	</li>
	<li>
		The second condition only lets pull requests coming from the {@render c('dev')} branch through.
		Pull requests from any other branch skip the job.
	</li>
	<li>{@render c('contents: read')} is enough, because the workflow only reads the code.</li>
</ul>
{@render code(conditionExample)}

<h2 class="mt-9 text-lg font-semibold">Steps</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	The workflow checks out the code and runs {@render c('./tests/tests.sh')}. If the script exits
	with an error, the check fails and the pull request is marked as failing.
</p>
{@render code(stepsExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		No dependencies are installed yet. If the tests need a tool or library, add an install step
		before {@render c('Run tests')}.
	</li>
	<li>
		The script must be executable ({@render c('chmod +x tests/tests.sh')}), or the step fails.
	</li>
	<li>The tests only run on pull requests, not on direct pushes to main or dev.</li>
</ul>