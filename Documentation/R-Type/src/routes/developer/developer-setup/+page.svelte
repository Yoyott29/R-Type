<script>
	const runExample = `./"Developer Setup/setup.sh"`;

	const stepsExample = `[1/4] Installing clang-format 21.1.8...
[2/4] Installing clang-tidy...
[3/4] Creating .vscode/settings.json...
[4/4] Enabling git hooks (commit message check)...`;

	const settingsExample = `{
    "[cpp]": {
        "editor.formatOnSave": true,
        "editor.defaultFormatter": "ms-vscode.cpptools"
    },
    "C_Cpp.formatting": "clangFormat",
    "C_Cpp.clang_format_style": "file:\${workspaceFolder}/Norms/.clang-format"
}`;

	const hooksExample = `git config core.hooksPath .githooks
chmod +x .githooks/*`;
</script>

<svelte:head>
	<title>Developer Setup - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">Developer Setup</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How to prepare your machine to work on R-Type with the setup script
</p>

<p class="max-w-[100ch] leading-7">
	Every developer runs the same script once after cloning the repository. It installs the code
	quality tools, configures VS Code and enables the git hooks, so your local checks match the ones
	run by the CI. The script is {@render c('Developer Setup/setup.sh')}.
</p>

<h2 class="mt-9 text-lg font-semibold">How to run it</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Run it from a terminal. The script moves to the repository root by itself, so it works from any
	folder. On Linux it uses {@render c('sudo')} when you are not root.
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Only <strong class="font-semibold">Linux</strong> and
	<strong class="font-semibold">Windows</strong> (Git Bash, MSYS or Cygwin) are supported. Any
	other system stops the script with an error. It is safe to run again: a tool that is already
	installed is skipped.
</p>

<h2 class="mt-9 text-lg font-semibold">What it does</h2>
{@render code(stepsExample)}

<h3 class="mt-6 font-semibold">1. clang-format (pinned version)</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	The script installs the exact version written in {@render c('CLANG_FORMAT_VERSION')}, with
	{@render c('pipx')} if available, or with {@render c('pip install --user')} otherwise. A pinned
	version matters because two versions can format the same file differently, and the CI would then
	reject code that looked correct on your machine. See the
	<a href="/developer/clang-format" class="text-[#ff8a3d] hover:underline">clang-format</a> page.
</p>

<h3 class="mt-6 font-semibold">2. clang-tidy</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	The tool is installed with the package manager of your system: {@render c('apt-get')},
	{@render c('dnf')} or {@render c('pacman')} on Linux, and {@render c('winget')} or
	{@render c('choco')} (LLVM package) on Windows. See the
	<a href="/developer/clang-tidy" class="text-[#ff8a3d] hover:underline">clang-tidy</a> page.
</p>

<h3 class="mt-6 font-semibold">3. VS Code settings</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	The script writes {@render c('.vscode/settings.json')} to format C++ files on save with the rules
	of {@render c('Norms/.clang-format')}. An existing file is first copied to
	{@render c('.vscode/settings.json.bak')}.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	If you are not using VS Code, the format-on-save configuration does not apply to you. Configure
	formatting in your own editor instead.
</p>
{@render code(settingsExample)}

<h3 class="mt-6 font-semibold">4. Git hooks</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	The script tells git to read hooks from the versioned {@render c('.githooks')} folder and makes
	them executable. See the
	<a href="/developer/githooks" class="text-[#ff8a3d] hover:underline">.githooks</a> page.
</p>
{@render code(hooksExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		If the script says your shell finds another clang-format version, add {@render c('~/.local/bin')}
		(Linux) or the Python {@render c('Scripts')} folder (Windows) at the
		<strong class="font-semibold">start</strong> of your PATH, then restart your terminal and VS Code.
	</li>
	<li>
		Install the <strong class="font-semibold">C/C++</strong> extension
		({@render c('ms-vscode.cpptools')}) in VS Code, or format on save will not work.
	</li>
	<li>
		{@render c('.vscode/')} is ignored by git, so these settings stay local to your machine.
	</li>
	<li>
		When the pinned version changes, update it both in {@render c('setup.sh')} and in
		{@render c('.github/workflows/tests.yml')}.
	</li>
</ul>