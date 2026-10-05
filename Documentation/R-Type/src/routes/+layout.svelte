<script>
	import './layout.css';
	import favicon from '$lib/assets/Favicon.png';
	import Sidebar from '$lib/components/Sidebar.svelte';
	import { afterNavigate } from '$app/navigation';

	let { children } = $props();

	let main = $state();

	afterNavigate(() => {
		main?.scrollTo(0, 0);
	});

	const userSections = [
		{ title: 'Introduction'},
	];

	const developerSections = [
		{ title: 'Introduction'},
		{ title: 'Github Wokflows'},
		{ title: 'Architecture'},
		{ title: 'Network'},
		{ title: 'Game'},
		{ title: 'Documentation'},
	];

	const userPages = [

		{ title: 'About', path: '/user/about', section: 0 },
		{ title: 'Getting Started', path: '/user/getting-started', section: 0 },
	];

	const developerPages = [
		{ title: 'About', path: '/developer/about', section: 0 },
		
		{ title: 'Auto Versioning', path: '/developer/auto-versioning', section: 1 },
		{ title: 'Documentation Deployment', path: '/developer/documentation-deployment', section: 1 },
		{ title: 'Mirror Repository', path: '/developer/mirror-repository', section: 1 },
		{ title: 'Running Tests', path: '/developer/running-tests', section: 1 },

		{ title: 'How to add a page', path: '/developer/add-a-page', type: 'Developer', section: 5 },
	];


</script>

<svelte:head><link rel="icon" href={favicon} /></svelte:head>

<div class="flex h-screen bg-[#0d1210] text-[#d8e4de]">
	<Sidebar {userSections} {developerSections} {userPages} {developerPages} />
	<main bind:this={main} class="min-w-0 flex-1 overflow-y-auto">
		<div class="mx-auto w-full max-w-5xl px-8 pt-12 pb-24 lg:px-16">
			{@render children()}
		</div>
	</main>
</div>

<style>
	.layout {
		display: flex;
	}
</style>
