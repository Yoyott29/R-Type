<script>
	const headerExample = `cmake_minimum_required(VERSION 3.28)

project(r-type_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \${CMAKE_SOURCE_DIR})`;

	const depsExample = `set(CPM_SOURCE_CACHE
    \${CMAKE_SOURCE_DIR}/.cpm-cache
    CACHE PATH "CPM download cache"
)

include(CPM.cmake)

CPMAddPackage(
    NAME raylib
    GITHUB_REPOSITORY raysan5/raylib
    GIT_TAG 5.5
    OPTIONS
        "BUILD_EXAMPLES OFF"
        "BUILD_SHARED_LIBS OFF"
)`;

	const targetExample = `add_executable(\${PROJECT_NAME}
    src/main.cpp
    src/game-rtype/Application.cpp
    src/game-rtype/RaylibRenderer.cpp
    src/engine/core/Scene.cpp
    ...
)`;

	const includeExample = `target_include_directories(\${PROJECT_NAME} PRIVATE
    src/game-rtype
    src/engine/core
    ...
)

target_include_directories(\${PROJECT_NAME} SYSTEM PRIVATE
    "\${raylib_SOURCE_DIR}/src"
)

target_link_libraries(\${PROJECT_NAME} PRIVATE raylib)`;

	const warnExample = `if(MSVC)
    target_compile_options(\${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(\${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()`;

	const addFileExample = `add_executable(\${PROJECT_NAME}
    ...
    src/systems/MovementSystem.cpp
    src/systems/CollisionSystem.cpp   # new file
)`;

	const buildExample = `cmake -B build
cmake --build build`;
</script>

<svelte:head>
	<title>How the CMakeLists works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How the CMakeLists works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	A walk through the root CMakeLists.txt, from configuration to executable
</p>

<p class="max-w-[100ch] leading-7">
	{@render c('CMakeLists.txt')} at the root of the repository describes how to build the game. CMake
	reads it to generate the build files, then the compiler produces the executable. Building takes two
	commands:
</p>
{@render code(buildExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The first command <strong class="font-semibold">configures</strong> the project: it reads the file
	and downloads dependencies. The second one <strong class="font-semibold">compiles</strong> it into
	the {@render c('build')} folder.
</p>

<h2 class="mt-9 text-lg font-semibold">1. Project settings</h2>
{@render code(headerExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>CMake 3.28 or newer is required.</li>
	<li>The project and its executable are named {@render c('r-type_client')}, and the language is C++.</li>
	<li>The code is compiled as C++17, and the compiler must support it.</li>
	<li>
		{@render c('CMAKE_RUNTIME_OUTPUT_DIRECTORY')} puts the executable at the
		<strong class="font-semibold">root of the repository</strong> instead of inside
		{@render c('build')}. It is ignored by git.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">2. Dependencies</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Dependencies are downloaded by CPM when CMake configures the project, so nothing has to be
	installed by hand. Here, raylib 5.5 is fetched from GitHub and built as a static library, without
	its examples. See
	<a href="/developer/cpm" class="text-[#ff8a3d] hover:underline">How CPM works</a>.
</p>
{@render code(depsExample)}

<h2 class="mt-9 text-lg font-semibold">3. The client target</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('add_executable')} lists every source file that makes up the client. The list is written
	by hand, there is no automatic search of the {@render c('src')} folder.
</p>
{@render code(targetExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	<strong class="font-semibold">When you create a new .cpp file, add it to this list</strong>, or it
	will not be compiled and the linker will report missing symbols.
</p>
{@render code(addFileExample)}

<h2 class="mt-9 text-lg font-semibold">4. Includes and linking</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The include directories let you write {@render c('#include "Scene.hpp"')} without the full path. The
	raylib headers are added as {@render c('SYSTEM')} includes, so warnings from raylib are not shown.
	{@render c('target_link_libraries')} links raylib into the client.
</p>
{@render code(includeExample)}

<h2 class="mt-9 text-lg font-semibold">5. Compiler warnings</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Warnings are enabled for the client, with the right flags for each compiler. They do not apply to
	raylib, whose warnings are disabled with {@render c('-w')}.
</p>
{@render code(warnExample)}

<h2 class="mt-9 text-lg font-semibold">6. clang-tidy</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The end of the file attaches clang-tidy to the client target. See the
	<a href="/developer/clang-tidy" class="text-[#ff8a3d] hover:underline">clang-tidy</a> page.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Only the client is defined. There is no server target, so
		{@render c('r-type_server')} is not built.
	</li>
	<li>
		If you change {@render c('CMakeLists.txt')}, running {@render c('cmake --build build')} again is
		enough: CMake reconfigures by itself.
	</li>
	<li>
		If the build behaves strangely, delete the {@render c('build')} folder and configure again.
	</li>
</ul>