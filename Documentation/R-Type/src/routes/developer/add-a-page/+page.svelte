<script>
	const tree = `src/routes/
├── +layout.svelte
├── user/
│   ├── about/
│   │   └── +page.svelte
│   └── getting-started/
│       └── +page.svelte
└── developer/
    ├── auto-versioning/
    │   └── +page.svelte
    └── my-page/
        └── +page.svelte`;

	const pageExample = `<svelte:head>
	<title>My page - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">My page</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Short subtitle</p>

<p class="max-w-[100ch] leading-7">Your content here.</p>`;

	const registerExample = `const developerPages = [
	{ title: 'Auto Versioning', path: '/developer/auto-versioning', section: 1 },
	{ title: 'My page', path: '/developer/my-page', section: 1 }
];`;

	const sectionExample = `const developerSections = [
	{ title: 'Introduction' },    // index 0
	{ title: 'Github Workflows' }, // index 1
	{ title: 'Architecture' },    // index 2
	{ title: 'Network' },         // index 3
	{ title: 'Game' },            // index 4
	{ title: 'Documentation' },   // index 5
	{ title: 'My new section' }   // index 6
];

const developerPages = [
	{ title: 'My page', path: '/developer/my-page', section: 6 }
];`;
</script>

<svelte:head>
	<title>How to add a page - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How to add a page</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">How to add a page to these docs</p>

<p class="max-w-[100ch] leading-7">
	The docs have two parts: <strong class="font-semibold">user</strong> pages and
	<strong class="font-semibold">developer</strong> pages. Each part has its own sections and its own
	list of pages in the sidebar.
</p>

<h2 class="mt-9 text-lg font-semibold">1. Create the route</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each folder under {@render c('src/routes/user')} or {@render c('src/routes/developer')} becomes a
	URL. Use lowercase and hyphens for the folder name, and put a {@render c('+page.svelte')} file
	inside.
</p>
{@render code(tree)}

<h2 class="mt-9 text-lg font-semibold">2. Write the content</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The layout already provides the sidebar, padding and width, so the page only needs its own
	content:
</p>
{@render code(pageExample)}

<h2 class="mt-9 text-lg font-semibold">3. Register it in the sidebar</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Open {@render c('src/routes/+layout.svelte')} and add an entry to {@render c('userPages')} or
	{@render c('developerPages')}. The {@render c('path')} must match the folder, including the
	{@render c('/user')} or {@render c('/developer')} prefix. The {@render c('section')} is the index
	of the section in {@render c('userSections')} or {@render c('developerSections')}, starting at 0.
</p>
{@render code(registerExample)}

<h2 class="mt-9 text-lg font-semibold">Adding a new section</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Add the section at the end of {@render c('userSections')} or {@render c('developerSections')},
	then point your pages at its index. Adding it at the end keeps the indexes of existing pages
	valid.
</p>
{@render code(sectionExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		User and developer lists are separate, so section 1 in {@render c('userPages')} and section 1
		in {@render c('developerPages')} are different sections.
	</li>
	<li>
		If you insert a section in the middle of the list, every page after it must have its
		{@render c('section')} index updated.
	</li>
	<li>A page that is not in {@render c('userPages')} or {@render c('developerPages')} still works by URL, but does not appear in the sidebar.</li>
</ul>