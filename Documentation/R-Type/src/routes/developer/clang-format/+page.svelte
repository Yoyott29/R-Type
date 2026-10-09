<script>
	const configExample = `BasedOnStyle: LLVM
IndentWidth: 4
ColumnLimit: 100
BreakBeforeBraces: Linux
InsertNewlineAtEOF: true`;

	const manualExample = `# format one file
clang-format --style=file:Norms/.clang-format -i src/main.cpp

# check a file without changing it
clang-format --style=file:Norms/.clang-format --dry-run -Werror src/main.cpp`;

	const allExample = `find src -name '*.cpp' -o -name '*.hpp' \\
  | xargs clang-format --style=file:Norms/.clang-format -i`;

	const ciExample = `clang-format --style=file:Norms/.clang-format --dry-run -Werror \\
  src/**/*.cpp src/**/*.hpp`;
</script>

<svelte:head>
	<title>clang-format - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">clang-format</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How the C++ code is formatted automatically
</p>

<p class="max-w-[100ch] leading-7">
	clang-format rewrites C++ files so that the whole project has the same style: same indentation,
	same line length, same brace placement. The rules are stored in
	{@render c('Norms/.clang-format')}.
</p>

<h2 class="mt-9 text-lg font-semibold">The rules</h2>
{@render code(configExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('BasedOnStyle: LLVM')}: start from the LLVM style, then override the options below.</li>
	<li>{@render c('IndentWidth: 4')}: indent with 4 spaces.</li>
	<li>{@render c('ColumnLimit: 100')}: lines are wrapped after 100 characters.</li>
	<li>{@render c('BreakBeforeBraces: Linux')}: Linux brace style, the brace of a function goes on its own line.</li>
	<li>{@render c('InsertNewlineAtEOF: true')}: every file ends with a newline.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">When it runs</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">When you save a file in VS Code.</strong> The
		<a href="/developer/developer-setup" class="text-[#ff8a3d] hover:underline">Developer Setup</a>
		script configures this.
	</li>
	<li>
		<strong class="font-semibold">In the CI.</strong> The {@render c('lint')} job of
		{@render c('tests.yml')} checks every {@render c('.cpp')} and {@render c('.hpp')} file in
		{@render c('src')}. A single difference fails the job and blocks the pull request. See
		<a href="/developer/running-tests" class="text-[#ff8a3d] hover:underline">Running Tests</a>.
	</li>
</ul>
{@render code(ciExample)}

<h2 class="mt-9 text-lg font-semibold">Running it by hand</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The configuration file is not at the root of the repository, so clang-format cannot find it on its
	own. Always pass its path with {@render c('--style=file:')}.
</p>
{@render code(manualExample)}
<p class="mt-3 max-w-[100ch] leading-7">To format every source file at once:</p>
{@render code(allExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The version is pinned to {@render c('21.1.8')}, in {@render c('setup.sh')} and in
		{@render c('tests.yml')}. Another version may format code differently and fail in the CI.
	</li>
	<li>Check your version with {@render c('clang-format --version')}.</li>
	<li>Run clang-format before pushing if your editor does not format on save.</li>
</ul>