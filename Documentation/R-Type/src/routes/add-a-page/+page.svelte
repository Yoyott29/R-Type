<script>
	const tree = `src/routes/
├── +layout.svelte
├── about/
│   └── +page.svelte
└── my-page/
    └── +page.svelte`;

	const pageExample = `<svelte:head>
	<title>My page - R-Type docs</title>
</svelte:head>

<h1 class="text-3xl font-semibold">My page</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Short subtitle</p>

<p class="max-w-[68ch] leading-7">Your content here.</p>`;

	const registerExample = `const pages = [
	{ title: 'About', path: '/about', section: 0 },
	{ title: 'My page', path: '/my-page', section: 0 }
];`;

	const sectionExample = `const sections = [
	{ title: 'Introduction' },   // index 0
	{ title: 'Architecture' },   // index 1
	{ title: 'My new section' }  // index 2
];

const pages = [
	{ title: 'My page', path: '/my-page', section: 2 }
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

<h1 class="text-3xl font-semibold">How to add a page</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">How to add a page to these docs</p>

<h2 class="mt-9 text-lg font-semibold">1. Create the route</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	Each folder under <code class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]">src/routes</code>
	becomes a URL. Use lowercase and hyphens for the folder name, and put a
	<code class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]">+page.svelte</code>
	file inside.
</p>
{@render code(tree)}

<h2 class="mt-9 text-lg font-semibold">2. Write the content</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	The layout already provides the sidebar, padding and width, so the page only needs its own
	content:
</p>
{@render code(pageExample)}

<h2 class="mt-9 text-lg font-semibold">3. Register it in the sidebar</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	Open <code class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]">src/routes/+layout.svelte</code>
	and add an entry to the <code class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]">pages</code>
	array. The <code class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]">path</code>
	must match the folder name, and <code class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]">section</code>
	is the index of the section in the <code class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]">sections</code>
	array, starting at 0.
</p>
{@render code(registerExample)}

<h2 class="mt-9 text-lg font-semibold">Adding a new section</h2>
<p class="mt-3 max-w-[68ch] leading-7">
	Add the section at the end of the
	<code class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]">sections</code>
	array, then point your pages at its index.
</p>
{@render code(sectionExample)}