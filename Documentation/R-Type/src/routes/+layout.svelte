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
		{ title: 'Developer Environment'},
		{ title: 'Build and Tests'},
		{ title: 'Github Workflows'},
		{ title: 'Documentation'},
	];

	const userPages = [

		{ title: 'About', path: '/user/about', section: 0 },
		{ title: 'Getting Started', path: '/user/getting-started', section: 0 },
	];

	const developerPages = [
		{ title: 'About', path: '/developer/about', section: 0 },
		
		
		{ title: 'Developer Setup', path: '/developer/developer-setup', section: 1 },
		{ title: '.githooks', path: '/developer/githooks', section: 1 },
		{ title: 'clang-format', path: '/developer/clang-format', section: 1 },
		{ title: 'clang-tidy', path: '/developer/clang-tidy', section: 1 },
		
		{ title: 'How CMakeLists works', path: '/developer/cmakelists', section: 2 },
		{ title: 'How CPM works', path: '/developer/cpm', section: 2 },
		{ title: 'How tests.sh works', path: '/developer/tests-sh', section: 2 },
		
		{ title: 'Auto Versioning', path: '/developer/auto-versioning', section: 3 },
		{ title: 'Deploy Documentation', path: '/developer/documentation-deployment', section: 3 },
		{ title: 'Mirror Repository', path: '/developer/mirror-repository', section: 3 },
		{ title: 'Running Tests', path: '/developer/running-tests', section: 3 },

		{ title: 'How to add a page', path: '/developer/add-a-page', type: 'Developer', section: 4 },
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
