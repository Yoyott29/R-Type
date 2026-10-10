<script>
	import { untrack } from 'svelte';
	import { page as current } from '$app/state';
	import { goto } from '$app/navigation';
	import { docMode } from '$lib/mode.svelte.js';

	let {
		userSections = [],
		developerSections = [],
		userPages = [],
		developerPages = []
	} = $props();

	const types = ['User', 'Developer'];

	function modeFor(path) {
		const inUser = userPages.some((p) => p.path === path);
		const inDev = developerPages.some((p) => p.path === path);
		return inDev && !inUser ? 'Developer' : 'User';
	}

	let mode = $state(modeFor(current.url.pathname));

	$effect(() => {
		docMode.current = mode;
	});

	$effect(() => {
		const path = current.url.pathname;
		untrack(() => {
			const inUser = userPages.some((p) => p.path === path);
			const inDev = developerPages.some((p) => p.path === path);

			if (mode === 'User' && !inUser && inDev) mode = 'Developer';
			else if (mode === 'Developer' && !inDev && inUser) mode = 'User';
		});
	});

	function switchMode(t) {
		if (t === mode) return;
		mode = t;
		goto('/');
	}

	const sections = $derived(mode === 'User' ? userSections : developerSections);
	const pages = $derived(mode === 'User' ? userPages : developerPages);
</script>

<div
	class="flex h-screen w-60 shrink-0 flex-col overflow-y-auto border-r border-[#1e2b26] bg-[#0a0e0c] py-7 [scrollbar-width:none] [&::-webkit-scrollbar]:hidden"
>
	<div class="mb-4 border-b border-[#1e2b26] px-6 pb-5 font-mono text-[13px] text-[#7f9189]">
		<strong class="font-semibold text-[#d8e4de]">R-Type</strong> / Documentation
	</div>

	<div class="px-6 pb-2">
		<div
			class="relative grid grid-cols-2 rounded-md border border-[#1e2b26] bg-[#0d1210] p-0.5 font-mono text-xs"
			role="tablist"
		>
			<span
				class="absolute inset-y-0.5 left-0.5 w-[calc(50%-2px)] rounded bg-[#ff8a3d]/15 transition-transform duration-200
					{mode === 'Developer' ? 'translate-x-full' : 'translate-x-0'}"
			></span>
			{#each types as t (t)}
				<button
					type="button"
					role="tab"
					aria-selected={mode === t}
					onclick={() => switchMode(t)}
					class="relative z-10 cursor-pointer py-1.5 text-center transition-colors
						{mode === t ? 'text-[#ff8a3d]' : 'text-[#7f9189] hover:text-[#d8e4de]'}"
				>
					{t}
				</button>
			{/each}
		</div>
	</div>

	{#each sections as section, n (section.title)}
		{@const sectionPages = pages.filter((p) => p.section === n)}

		<div class="flex flex-col">
			<h3 class="mt-5 px-6 py-1 font-mono text-lg font-bold text-[#d8e4de]">
				{section.title}
			</h3>

			{#each sectionPages as p, j (p.title)}
				{@const active = current.url.pathname === p.path}
				<button
					type="button"
					onclick={() => goto(p.path)}
					class="flex w-full cursor-pointer items-center justify-between border-l-2 px-6 py-[7px] text-left text-sm transition-colors
						{active
						? 'border-[#ff8a3d] bg-[#ff8a3d]/[0.06] text-[#ff8a3d]'
						: 'border-transparent text-[#7f9189] hover:text-[#d8e4de]'}"
				>
					<span>{p.title}</span>
					{#if active}
						<span class="font-mono text-xs">{n + 1}.{j + 1}</span>
					{/if}
				</button>
			{/each}
		</div>
	{/each}
</div>