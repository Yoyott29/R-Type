<script>
	const activateExample = `git config core.hooksPath .githooks`;

	const formatExample = `[name] [branch] Type: message`;

	const goodExample = `[E.DU] [Test_Branch] Feat: added a test`;

	const regexExample = `^\\[[A-Z]\\.[A-Z]{2}\\] \\[[^]]+\\] (Feat|Fix|Refactor|Docs|Style|Chore|Merge): .+`;

	const errorExample = `Invalid commit message:
  added a test

Expected: [name] [branch] Type: message
Example:  [E.DU] [Test_Branch] Feat: added a test
Types:    Feat, Fix, Refactor, Docs, Style, Chore, Merge`;

	const branchExample = `Branch name mismatch:
  message says: [Test_Branch]
  current branch: [Other_Branch]`;

	const addExample = `touch .githooks/pre-commit
chmod +x .githooks/pre-commit`;
</script>

<svelte:head>
	<title>.githooks - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">.githooks</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	The git hooks that check every commit made in the repository
</p>

<p class="max-w-[100ch] leading-7">
	The {@render c('.githooks')} folder contains the git hooks of the project. A hook is a script that
	git runs automatically at a given moment, for example when a commit message is written. Because the
	folder is part of the repository, every developer shares the same hooks.
</p>

<h2 class="mt-9 text-lg font-semibold">How hooks are enabled</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	By default git reads hooks from {@render c('.git/hooks')}, which is not versioned. The
	<a href="/developer/developer-setup" class="text-[#ff8a3d] hover:underline">Developer Setup</a>
	script changes this setting for your clone:
</p>
{@render code(activateExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	If you do not run the setup script, run this command yourself and make sure the hook files are
	executable.
</p>

<h2 class="mt-9 text-lg font-semibold">The commit-msg hook</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('.githooks/commit-msg')} runs after you write a commit message and before the commit is
	created. If the message is rejected, no commit is made. It reads the first line of the message and
	checks two things.
</p>

<h3 class="mt-6 font-semibold">1. The format</h3>
{@render code(formatExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">name</strong>: your initials, one capital letter, a dot, then two
		capital letters.
	</li>
	<li><strong class="font-semibold">branch</strong>: the name of your branch.</li>
	<li>
		<strong class="font-semibold">Type</strong>: one of {@render c('Feat')}, {@render c('Fix')},
		{@render c('Refactor')}, {@render c('Docs')}, {@render c('Style')}, {@render c('Chore')} or
		{@render c('Merge')}.
	</li>
	<li><strong class="font-semibold">message</strong>: a short description, after a colon and a space.</li>
</ul>
{@render code(goodExample)}
<p class="mt-3 max-w-[100ch] leading-7">The check is done with this regular expression:</p>
{@render code(regexExample)}
{@render code(errorExample)}

<h3 class="mt-6 font-semibold">2. The branch name</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	The branch written in the message must be the branch you are really on, given by
	{@render c('git rev-parse --abbrev-ref HEAD')}. This avoids commits labeled with the wrong
	branch. The check is skipped when git reports {@render c('HEAD')} (detached HEAD).
</p>
{@render code(branchExample)}

<h2 class="mt-9 text-lg font-semibold">Commits that are not checked</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Messages that start with {@render c('Merge ')}, {@render c('Revert ')}, {@render c('fixup! ')} or
	{@render c('squash! ')} pass without any check, because git generates them.
</p>

<h2 class="mt-9 text-lg font-semibold">Adding a hook</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Create a file named after the git hook (for example {@render c('pre-commit')}) in
	{@render c('.githooks')} and make it executable. Developers who already ran the setup script get it
	on their next pull.
</p>
{@render code(addExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>A hook that is not executable is silently ignored by git.</li>
	<li>
		{@render c('git commit --no-verify')} skips the hooks, so do not rely on them as the only
		protection.
	</li>
</ul>