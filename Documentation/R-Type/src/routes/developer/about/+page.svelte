<script>
	const treeExample = `R-Type/
├── .github/workflows/     # automation
├── Documentation/R-Type/  # this documentation site
├── src/                   # source code
├── tests/                 # tests (tests/tests.sh)
├── CMakeLists.txt         # build configuration
└── Readme.md`;
</script>

<svelte:head>
	<title>About - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">About</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	An overview of the R-Type project for developers
</p>

<p class="max-w-[100ch] leading-7">
	This is the developer documentation for R-Type. It explains how the repository is organized, how
	code moves from one branch to another, and which parts are automated. If you want to play the
	game or install it, see the user documentation instead.
</p>

<h2 class="mt-9 text-lg font-semibold">Repository layout</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The project lives in one repository, with the code, the tests, the automation and this
	documentation side by side:
</p>
{@render code(treeExample)}

<h2 class="mt-9 text-lg font-semibold">Branches and releases</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">main</strong> holds the stable version of the project. Every
		major release (v1.0, v2.0...) is created from it.
	</li>
	<li>
		<strong class="font-semibold">dev</strong> is where work is integrated. Changes reach main
		through a pull request from dev, and the tests run on that pull request.
	</li>
	<li>
		Any other branch can create a pre-release (v1.1, v1.2...) when the versioning workflow is run on
		it.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Automation</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Four GitHub Actions workflows in {@render c('.github/workflows')} handle the repetitive work:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render a('/developer/auto-versioning', 'Auto Versioning')}: numbers and publishes GitHub
		releases.
	</li>
	<li>
		{@render a('/developer/running-tests', 'Running Tests')}: runs the test script on pull requests
		from dev to main.
	</li>
	<li>
		{@render a('/developer/mirror-repository', 'Mirror Repository')}: copies every branch and tag
		to the Epitech repository.
	</li>
	<li>
		{@render a('/developer/documentation-deployment', 'Documentation Deployment')}: publishes this
		site to Vercel when the docs change on main.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Contributing to the docs</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The documentation is a SvelteKit site in {@render c('Documentation/R-Type')}. To add or change a
	page, follow {@render a('/developer/add-a-page', 'How to add a page')}.
</p>