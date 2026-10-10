<script>
	const configExample = `Checks: >
     bugprone-*,
     performance-*,
     -bugprone-easily-swappable-parameters
WarningsAsErrors: ''`;

	const cmakeExample = `option(ENABLE_CLANG_TIDY "Run clang-tidy during the build" ON)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)

    if(CLANG_TIDY_EXE)
        set_target_properties(\${PROJECT_NAME} PROPERTIES
            CXX_CLANG_TIDY
                "\${CLANG_TIDY_EXE};--config-file=\${CMAKE_SOURCE_DIR}/Norms/.clang-tidy"
        )
    else()
        message(WARNING "clang-tidy not found: static analysis skipped")
    endif()
endif()`;

	const offExample = `cmake -B build -DENABLE_CLANG_TIDY=OFF
cmake --build build`;

	const ciExample = `cmake -B build-tidy -DENABLE_CLANG_TIDY=ON
cmake --build build-tidy --parallel`;

	const suppressExample = `// NOLINT silences every check on this line
int value = compute(); // NOLINT

// silence one check only
int other = compute(); // NOLINT(bugprone-narrowing-conversions)`;
</script>

<svelte:head>
	<title>clang-tidy - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">clang-tidy</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How static analysis finds bugs while the project compiles
</p>

<p class="max-w-[100ch] leading-7">
	clang-tidy reads the C++ code and reports likely bugs and slow patterns that the compiler does not
	flag. In R-Type it runs during the build, so problems show up in the same output as compiler
	warnings.
</p>

<h2 class="mt-9 text-lg font-semibold">The configuration</h2>
<p class="mt-3 max-w-[100ch] leading-7">The checks are chosen in {@render c('Norms/.clang-tidy')}:</p>
{@render code(configExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('bugprone-*')}: checks for code that is probably a bug.</li>
	<li>{@render c('performance-*')}: checks for unnecessary copies and other avoidable costs.</li>
	<li>
		{@render c('-bugprone-easily-swappable-parameters')}: the leading minus disables this check, which
		complains about functions with several parameters of the same type.
	</li>
	<li>
		{@render c("WarningsAsErrors: ''")}: nothing is promoted to an error. Findings are warnings and
		do not stop the build.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">How it is plugged into CMake</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	CMake sets the {@render c('CXX_CLANG_TIDY')} property on the client target, so clang-tidy runs on each
	source file right after it is compiled. Only the project files are analyzed, not raylib.
</p>
{@render code(cmakeExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The option {@render c('ENABLE_CLANG_TIDY')} is {@render c('ON')} by default.
	</li>
	<li>
		If clang-tidy is not installed, CMake prints a warning and builds without it. The
		<a href="/developer/developer-setup" class="text-[#ff8a3d] hover:underline">Developer Setup</a>
		script installs it.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Turning it off locally</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Analysis makes each compilation slower. To skip it while you iterate, configure with the option
	set to {@render c('OFF')}:
</p>
{@render code(offExample)}

<h2 class="mt-9 text-lg font-semibold">In the CI</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	After the tests, the {@render c('tests')} job builds the project again in a separate folder with
	analysis enabled. See
	<a href="/developer/running-tests" class="text-[#ff8a3d] hover:underline">Running Tests</a>.
</p>
{@render code(ciExample)}

<h2 class="mt-9 text-lg font-semibold">Silencing a warning</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	When a finding is a false positive, add a {@render c('NOLINT')} comment on the line. Prefer naming the
	check, so other problems on the same line are still reported.
</p>
{@render code(suppressExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The first build is the slowest, because every file is compiled and analyzed.
	</li>
	<li>
		CMake also writes {@render c('compile_commands.json')} in the build folder
		({@render c('CMAKE_EXPORT_COMPILE_COMMANDS')}), which editors and clang tools use to understand how
		each file is compiled.
	</li>
</ul>