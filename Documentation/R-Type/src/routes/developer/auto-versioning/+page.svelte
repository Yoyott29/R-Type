<script>
	const tagsExample = `git fetch --tags --force

ALL_TAGS="$(git tag -l 'v*' --sort=-version:refname | grep -E '^v[0-9]+\\.[0-9]+$' || true)"
LAST_TAG="$(echo "$ALL_TAGS" | head -n 1)"`;

const triggerExample = `on:
  push:
    branches:
      - main
  workflow_dispatch:`;
</script>

<svelte:head>
	<title>Auto Versioning - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">Auto Versioning</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How the auto-version workflow numbers and publishes releases
</p>

<p class="max-w-[100ch] leading-7">
	The workflow {@render c('.github/workflows/auto-version.yml')} creates a GitHub Release with a new
	tag of the form {@render c('vX.Y')}. Both release types are created from {@render c('main')}:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		A push to <strong class="font-semibold">main</strong>, including a merge, creates the next minor
		version (v1.1, v1.2, v1.3...).
	</li>
	<li>
		A manual run from the Actions tab creates the next major version (v2.0, v3.0, v4.0...).
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">When it runs</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The workflow runs automatically when code is pushed to {@render c('main')}. It can also be started
	manually with {@render c('workflow_dispatch')} from the <strong class="font-semibold">Actions</strong>
	tab. The script validates that the selected ref is {@render c('main')}, so manual runs must use that
	branch.
</p>
{@render code(triggerExample)}

<h2 class="mt-9 text-lg font-semibold">Settings</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('contents: write')} lets the workflow create tags and releases.</li>
	<li>{@render c('PROJECT_NAME')} is used in release titles, for example "R-Type v1.1".</li>
	<li>{@render c('concurrency')} queues overlapping runs without cancelling them, so versions are computed in order.</li>
	<li>{@render c('runs-on: ubuntu-latest')} runs the job on a GitHub-hosted Ubuntu runner.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">How it works</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The workflow checks out the full history ({@render c('fetch-depth: 0')}) so it can see all
	previous tags and commits. It then finds the latest tag matching {@render c('vX.Y')}. If there is
	none, it starts from v1.0, so the first automatic release is v1.1.
</p>
{@render code(tagsExample)}

<p class="mt-5 max-w-[100ch] leading-7">
	For an automatic run, the major number stays the same and the minor number increases by one. For a
	manual run, the major number increases by one and the minor resets to 0.
</p>

<p class="mt-5 max-w-[100ch] leading-7">
	The release notes depend on the trigger. A major version lists the minor versions published during
	the previous major version. A minor version lists up to 50 commit messages since the last tag,
	excluding merge commits. The initial minor release lists up to 50 commits from the repository
	history.
</p>

<p class="mt-5 max-w-[100ch] leading-7">
	Finally, {@render c('gh release create')} tags the commit that started the run and publishes the
	release with its generated title and notes. Every release is marked as the latest release with
	{@render c('--latest')}.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Only tags in the exact format {@render c('vX.Y')} are counted. Tags like v1.2.3 are ignored.</li>
	<li>The workflow uses {@render c('GITHUB_TOKEN')} through {@render c('GH_TOKEN')} to create releases.</li>
	<li>Running the workflow twice creates two versions, even when both runs target the same commit.</li>
</ul>