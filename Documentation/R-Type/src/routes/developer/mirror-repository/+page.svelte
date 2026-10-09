<script>
	const triggerExample = `on:
  push:
    branches: ['**']
    tags: ['**']
  delete:
  workflow_dispatch:`;

	const conditionExample = `env:
  TARGET_REPO: EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4

jobs:
  mirror:
    if: github.repository != 'EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4'`;

	const sshExample = `install -m 700 -d ~/.ssh
printf '%s\\n' "$SSH_KEY" > ~/.ssh/id_mirror
chmod 600 ~/.ssh/id_mirror
ssh-keyscan github.com >> ~/.ssh/known_hosts`;

	const fetchExample = `git fetch origin '+refs/heads/*:refs/remotes/origin/*' --prune --tags --force

if [[ -z "$(git for-each-ref refs/remotes/origin)" ]]; then
  echo "::error::No branches found on the mirror, aborting."
  exit 1
fi`;

	const pushExample = `git push "$TARGET" --force --prune \\
  'refs/remotes/origin/*:refs/heads/*' \\
  'refs/tags/*:refs/tags/*'`;
</script>

<svelte:head>
	<title>Mirror to Epitech - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">Mirror to Epitech</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How every branch and tag is copied to the Epitech repository
</p>

<p class="max-w-[68ch] leading-7">
	The workflow {@render c('.github/workflows/mirror-repo.yml')} keeps the Epitech repository
	identical to this one. After each change, it copies all branches and tags to the target
	repository, including deletions.
</p>

<h2 class="mt-9 text-lg font-semibold">When it runs</h2>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>On every push to any branch or tag.</li>
	<li>When a branch or tag is deleted, so the deletion is copied too.</li>
	<li>Manually, from the <strong class="font-semibold">Actions</strong> tab.</li>
</ul>
{@render code(triggerExample)}

<h2 class="mt-9 text-lg font-semibold">Settings</h2>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('TARGET_REPO')} is the repository that receives the copy.</li>
	<li>
		{@render c('if')} skips the job on the target repository itself, so it never mirrors onto
		itself.
	</li>
	<li>
		{@render c('contents: read')} is enough, because the push to the target uses an SSH key, not
		the GitHub token.
	</li>
	<li>
		{@render c('concurrency')} makes runs wait for each other, so two syncs never push at the same
		time. A run in progress is not cancelled.
	</li>
</ul>
{@render code(conditionExample)}

<h2 class="mt-9 text-lg font-semibold">Secret</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	The repository needs one secret, {@render c('GIT_SSH_PRIVATE_KEY')}, set in
	<strong class="font-semibold">Settings &gt; Secrets and variables &gt; Actions</strong>. It must
	be a private SSH key whose public key is added to the target repository with write access.
</p>

<h2 class="mt-9 text-lg font-semibold">How it works</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	The workflow checks out the full history, then writes the SSH key to a file and trusts GitHub's
	host key.
</p>
{@render code(sshExample)}

<p class="mt-5 max-w-[68ch] leading-7">
	Next it fetches every branch and tag of this repository and drops the ones that no longer exist.
	If the fetch returns no branches at all, the run stops with an error instead of wiping the
	target.
</p>
{@render code(fetchExample)}

<p class="mt-5 max-w-[68ch] leading-7">
	Finally it pushes everything to the target. {@render c('--force')} overwrites rewritten history,
	and {@render c('--prune')} deletes branches and tags on the target that no longer exist here.
</p>
{@render code(pushExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[68ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The copy is one-way. Anything pushed directly to the target repository is overwritten or
		deleted on the next sync.
	</li>
	<li>Only branches and tags are copied, not issues, pull requests or releases.</li>
	<li>If the SSH key is missing or has no write access, the push fails and nothing is synced.</li>
</ul>