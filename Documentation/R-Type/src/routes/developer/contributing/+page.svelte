<script>
	const flowExample = `main      stable, every merge creates a release (v1.1, v1.2...)
  ^
  | pull request (runs the tests and lint workflows)
dev       integration branch
  ^
  | pull request, reviewed by another member
Add_Player_Shooting, Refactor_Code, ...   one branch per feature or fix`;

	const commitExample = `[name] [branch] Type: message

[C.NE] [Add_Ip_Port_Args] Feat: added the ip and port arguments to the server
[A.PI] [Add_Tests] Fix: fix the test in tests/tests.sh
[E.DU] [dev] Docs: updated the protocol page

name    first letter of the first name, dot, first two letters of the last name, in capitals
branch  the current branch, checked by the hook
Type    Feat, Fix, Refactor, Docs, Style, Chore or Merge`;

	const namingExample = `classes, structs, enums     PascalCase            Scene, ComponentPool, EntType
functions, methods          camelCase             createEntity, sendSnapshot
private members             _camelCase            _entityManager, _playersMutex
constants and macros        UPPER_SNAKE_CASE      MAX_ENTITIES, PROTOCOL_VERSION
engine and plugin files     PascalCase.hpp/.cpp   Scene.hpp, RayLibRenderer.cpp
server files                snake_case.hpp/.cpp   game_loop.cpp, input_queue.hpp
headers                     #pragma once
namespaces                  rtype_server, rtype_protocol`;

	const checklistExample = `[ ] the branch starts from an up to date dev
[ ] ./tests/tests.sh passes
[ ] the code is formatted (format on save, or clang-format with Norms/.clang-format)
[ ] no new clang-tidy warning in the build
[ ] new .cpp files are added to CMakeLists.txt
[ ] the documentation pages of the changed code are updated
[ ] PROTOCOL_VERSION is raised if a packet changed`;
</script>

<svelte:head>
	<title>Contributing - R-Type docs</title>
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

{#snippet a(href, text)}<a {href} class="text-[#ff8a3d] hover:underline">{text}</a>{/snippet}

<h1 class="text-3xl font-semibold">Contributing</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	The team workflow and the coding conventions to follow
</p>

<p class="max-w-[100ch] leading-7">
	Start with {@render a('/developer/developer-setup', 'Developer Setup')}: it installs the formatter and
	clang-tidy, and enables the commit message hook. Then follow the steps below for every change.
</p>

<h2 class="mt-9 text-lg font-semibold">1. Pick or open an issue</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each feature or bug starts as a GitHub issue that describes the goal. Assign it to yourself before
	starting, so two members do not work on the same thing.
</p>

<h2 class="mt-9 text-lg font-semibold">2. Create a branch from dev</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Name the branch after what it does, with words in capitals separated by underscores, starting with
	a verb: {@render c('Add_Enemy_Waves')}, {@render c('Refactor_Code')},
	{@render c('Encapsulate_Remaining_Raylib')}. Never commit a feature directly on main.
</p>
{@render code(flowExample)}

<h2 class="mt-9 text-lg font-semibold">3. Commit</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Make small commits that each do one thing. The {@render c('.githooks/commit-msg')} hook rejects a
	message that does not follow this format (see {@render a('/developer/githooks', '.githooks')}):
</p>
{@render code(commitExample)}

<h2 class="mt-9 text-lg font-semibold">4. Open a pull request</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Open the pull request towards dev, link the issue, and say how you tested the change.</li>
	<li>Another member reviews it before it is merged.</li>
	<li>
		To release, open a pull request from dev to main. The tests and lint workflows run on it, and the
		merge creates the next version automatically (see
		{@render a('/developer/auto-versioning', 'Auto Versioning')}).
	</li>
</ul>
{@render code(checklistExample)}

<h2 class="mt-9 text-lg font-semibold">Coding conventions</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Formatting is not a matter of taste: {@render c('Norms/.clang-format')} decides it (LLVM base, 4
	spaces, 100 columns, Linux braces), and the CI fails if a file is not formatted. clang-tidy checks
	the rules in {@render c('Norms/.clang-tidy')}. For what the tools do not check, follow the existing
	code:
</p>
{@render code(namingExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>Write code, comments and documentation in English.</li>
	<li>Every file starts with the Epitech header.</li>
	<li>
		Report misuse with the exceptions of {@render c('src/engine/errors/EngineError.hpp')}, and catch
		them where the program can go on (a packet, a frame), never with an empty {@render c('catch')}.
	</li>
	<li>
		Keep the layers apart: the engine core must not include a plugin, raylib or Asio. See
		{@render a('/developer/architecture', 'Architecture')}.
	</li>
	<li>
		A new component goes in {@render c('src/components')} (shared) or in the folder of its plugin; a
		new system in {@render c('src/systems')} or in its plugin. See
		{@render a('/developer/how-to', 'How-to guides')}.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Some older files do not follow every rule yet (a few French comments, some methods in
		PascalCase such as {@render c('Game::SpawnEnemy')}). Fix them when you touch them.
	</li>
	<li>
		Documentation changes follow the same flow; see
		{@render a('/developer/add-a-page', 'How to add a page')}.
	</li>
</ul>
