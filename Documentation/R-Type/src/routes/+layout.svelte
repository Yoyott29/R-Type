<script>
	import './layout.css';
	import favicon from '$lib/assets/Favicon.png';
	import Sidebar from '$lib/components/Sidebar.svelte';
	import { afterNavigate } from '$app/navigation';
	import { docProject } from '$lib/project.svelte.js';

	let { children } = $props();

	let main = $state();

	// The accent color follows the selected project (see the data-project rules in layout.css)
	$effect(() => {
		document.documentElement.dataset.project = docProject.current;
	});

	afterNavigate(() => {
		main?.scrollTo(0, 0);
	});
</script>

<svelte:head>
	<link rel="icon" href={favicon} />
	<meta
		name="description"
		content="Documentation of R-Type, a networked shoot 'em up in C++20 built with the Gringo ECS engine: how to play, the architecture, the UDP protocol and the development workflow."
	/>
</svelte:head>

<a
	href="#content"
	class="sr-only z-50 rounded bg-accent px-3 py-2 text-[#0d1210] focus:not-sr-only focus:fixed focus:top-2 focus:left-2"
	>Skip to content</a
>

<div class="flex h-screen bg-[#0d1210] text-[#d8e4de]">
	<Sidebar />
	<main id="content" tabindex="-1" bind:this={main} class="min-w-0 flex-1 overflow-y-auto">
		<div class="mx-auto w-full max-w-5xl px-8 pt-12 pb-24 lg:px-16">
			{@render children()}
		</div>
	</main>
</div>
