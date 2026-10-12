<script>
	const triggerExample = `on:
  workflow_dispatch:`;

	const tagsExample = `git fetch --tags --force

ALL_TAGS="$(git tag -l 'v*' --sort=-version:refname | grep -E '^v[0-9]+\\.[0-9]+$' || true)"
LAST_TAG="$(echo "$ALL_TAGS" | head -n 1)"`;

	const branchExample = `if [[ "$BRANCH" == "main" ]]; then
  # major version: v1.0, v2.0, v3.0...
  NEW_TAG="v$((LAST_MAJOR + 1)).0"
  PRERELEASE="false"
else
  # minor version: v1.1, v1.2, v1.3...
  NEW_TAG="v\${LAST_MAJOR}.$((LAST_MINOR + 1))"
  PRERELEASE="true"
fi`;

	const releaseExample = `gh release create "$RELEASE_TAG" \\
  --target "$GITHUB_SHA" \\
  --title "$RELEASE_TITLE" \\
  --notes-file release_notes.md \\
  "\${FLAGS[@]}"`;

	const enableExample = `on:
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
	tag of the form {@render c('vX.Y')}. The branch decides the type of version:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">main</strong>: a major version (v1.0, v2.0...), published as the
		latest release.
	</li>
	<li>
		<strong class="font-semibold">dev</strong>: a minor version (v1.1, v1.2...),
		published as a pre-release.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">When it runs</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The workflow is currently started manually with {@render c('workflow_dispatch')} from the
	<strong class="font-semibold">Actions</strong> tab. Select the branch or commit to release, then
	run the workflow.
</p>
{@render code(enableExample)}

<h2 class="mt-9 text-lg font-semibold">Settings</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('contents: write')} lets the workflow create tags and releases.</li>
	<li>{@render c('PROJECT_NAME')} is used in release titles, for example "R-Type v1.0".</li>
	<li>{@render c('concurrency')} makes runs wait for each other, so two runs never compute the same version.</li>
	<li>
		{@render c('runs-on: self-hosted')} runs the job on our own runner. If it is offline, the run
		waits.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">How it works</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The workflow checks out the full history ({@render c('fetch-depth: 0')}) so it can see all
	previous tags and commits. It then finds the latest tag matching {@render c('vX.Y')}. If there is
	none, it starts from 0.0.
</p>
{@render code(tagsExample)}

<p class="mt-5 max-w-[100ch] leading-7">
	On <strong class="font-semibold">main</strong> the major number goes up and the minor resets to 0.
	On dev the minor number goes up by one.
</p>
{@render code(branchExample)}

<p class="mt-5 max-w-[100ch] leading-7">
	The release notes depend on the version type. A major version lists the minor versions published
	since the previous major one. A minor version lists the commit messages since the last tag (merge
	commits excluded, 50 lines max).
</p>

<p class="mt-5 max-w-[100ch] leading-7">
	Finally, {@render c('gh release create')} tags the commit that started the run and publishes the
	release with the generated title and notes, using {@render c('--prerelease')} for minor versions
	and {@render c('--latest')} for major ones.
</p>
{@render code(releaseExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Only tags in the exact format {@render c('vX.Y')} are counted. Tags like v1.2.3 are ignored.</li>
	<li>Running the workflow twice on the same commit creates two versions.</li>
</ul>