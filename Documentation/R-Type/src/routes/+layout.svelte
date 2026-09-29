<script>
	import './layout.css';
	import favicon from '$lib/assets/Favicon.png';
	import Sidebar from '$lib/components/Sidebar.svelte';

	let { children } = $props();

	const sections = [
		{ title: 'Introduction' },
		{ title: 'Architecture' },
		{ title: 'Network' },
		{ title: 'Game' },
		{ title: 'Documentation' },
	];

	const pages = [
		{ title: 'About', path: '/about', section: 0 },
		{ title: 'Getting Started', path: '/getting-started', section: 0 },
		{ title: 'How to add a page', path: '/add-a-page', section: 4 },
	];
</script>

<svelte:head><link rel="icon" href={favicon} /></svelte:head>

<div class="flex h-screen bg-[#0d1210] text-[#d8e4de]">
	<Sidebar sections={sections} pages={pages} />
	<main
		class="max-w-3xl flex-1 overflow-y-auto px-14 pt-12 pb-20"
	>{@render children()}
	</main>
</div>

<style>
	.layout {
		display: flex;
	}
</style>
