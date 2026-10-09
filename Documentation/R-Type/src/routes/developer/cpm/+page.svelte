<script>
	const bootstrapExample = `set(CPM_DOWNLOAD_VERSION 0.43.2)
set(CPM_HASH_SUM "49a3bef9...")

set(CPM_DOWNLOAD_LOCATION "\${CMAKE_BINARY_DIR}/cmake/CPM_\${CPM_DOWNLOAD_VERSION}.cmake")

file(DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v\${CPM_DOWNLOAD_VERSION}/CPM.cmake
    \${CPM_DOWNLOAD_LOCATION}
    EXPECTED_HASH SHA256=\${CPM_HASH_SUM}
)

include(\${CPM_DOWNLOAD_LOCATION})`;

	const addExample = `CPMAddPackage(
    NAME raylib
    GITHUB_REPOSITORY raysan5/raylib
    GIT_TAG 5.5
    OPTIONS
        "BUILD_EXAMPLES OFF"
        "BUILD_SHARED_LIBS OFF"
)`;

	const cacheExample = `set(CPM_SOURCE_CACHE
    \${CMAKE_SOURCE_DIR}/.cpm-cache
    CACHE PATH "CPM download cache"
)`;

	const newDepExample = `CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 11.0.2
)

target_link_libraries(\${PROJECT_NAME} PRIVATE fmt::fmt)`;

	const ciExample = `- name: Restore CPM cache
  uses: actions/cache@v4
  with:
    path: .cpm-cache
    key: cpm-\${{ runner.os }}-\${{ hashFiles('CMakeLists.txt', 'CPM.cmake') }}`;
</script>

<svelte:head>
	<title>How CPM works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How CPM works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How dependencies like raylib are downloaded and built by CMake
</p>

<p class="max-w-[100ch] leading-7">
	<strong class="font-semibold">CPM</strong> (CMake Package Manager) is a small CMake script that
	downloads libraries from GitHub while the project is configured. That is why a fresh clone only needs
	CMake and a compiler: raylib is fetched and compiled for you, on Linux and on Windows.
</p>

<h2 class="mt-9 text-lg font-semibold">The bootstrap file</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('CPM.cmake')} at the repository root does not contain CPM itself. It downloads a pinned
	release of CPM into the build folder, checks its SHA256 hash, then includes it. If the downloaded
	file does not match the hash, configuration fails.
</p>
{@render code(bootstrapExample)}

<h2 class="mt-9 text-lg font-semibold">Declaring a dependency</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('CPMAddPackage')} in {@render c('CMakeLists.txt')} tells CPM what to fetch. For raylib, it
	downloads tag {@render c('5.5')} from GitHub and builds it as a static library without examples.
	CPM then makes the {@render c('raylib')} target and the {@render c('raylib_SOURCE_DIR')} variable
	available, which the project uses to link the library and find its headers.
</p>
{@render code(addExample)}

<h2 class="mt-9 text-lg font-semibold">The download cache</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('CPM_SOURCE_CACHE')} makes CPM store downloaded sources in {@render c('.cpm-cache')} at the
	repository root. The next configuration reuses them instead of downloading again, even after you
	delete the {@render c('build')} folder. The folder is ignored by git.
</p>
{@render code(cacheExample)}
<p class="mt-3 max-w-[100ch] leading-7">The CI caches this same folder between runs. The cache is renewed when a dependency changes.</p>
{@render code(ciExample)}

<h2 class="mt-9 text-lg font-semibold">Adding or updating a dependency</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-5 leading-7">
	<li>Add a {@render c('CPMAddPackage')} block in {@render c('CMakeLists.txt')}.</li>
	<li>Link the library with {@render c('target_link_libraries')}.</li>
	<li>
		To update a version, change {@render c('GIT_TAG')}. The CI cache key includes
		{@render c('CMakeLists.txt')}, so it is refreshed automatically.
	</li>
</ol>
<p class="mt-3 max-w-[100ch] leading-7">A generic example with another library:</p>
{@render code(newDepExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>The first configuration needs an internet connection, since it downloads CPM and raylib.</li>
	<li>
		To update CPM itself, change both {@render c('CPM_DOWNLOAD_VERSION')} and
		{@render c('CPM_HASH_SUM')} in {@render c('CPM.cmake')}.
	</li>
	<li>
		On Linux, raylib still needs the OpenGL and X11 development packages, which CPM cannot install.
		The CI installs them with apt.
	</li>
</ul>