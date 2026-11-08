<script>
	import { untrack } from 'svelte';
	import { page as current } from '$app/state';
	import { goto } from '$app/navigation';
	import { docMode } from '$lib/mode.svelte.js';
	import { docProject } from '$lib/project.svelte.js';
	import {
		userPages,
		developerPages,
		trackntrapUserPages,
		trackntrapDeveloperPages,
		gringoUserPages,
		gringoDeveloperPages,
		sidebarFor,
		projectOf
	} from '$lib/pages.js';

	const projects = ['R-Type', 'TrackNTrap', 'Gringo'];
	const types = ['User', 'Developer'];
	const allUserPages = [...userPages, ...trackntrapUserPages, ...gringoUserPages];
	const allDeveloperPages = [...developerPages, ...trackntrapDeveloperPages, ...gringoDeveloperPages];

	function modeFor(path) {
		const inUser = allUserPages.some((p) => p.path === path);
		const inDev = allDeveloperPages.some((p) => p.path === path);
		return inDev && !inUser ? 'Developer' : 'User';
	}

	let mode = $state(modeFor(current.url.pathname));

	$effect(() => {
		docMode.current = mode;
	});

	$effect(() => {
		const path = current.url.pathname;
		untrack(() => {
			const inUser = allUserPages.some((p) => p.path === path);
			const inDev = allDeveloperPages.some((p) => p.path === path);

			if (mode === 'User' && !inUser && inDev) mode = 'Developer';
			else if (mode === 'Developer' && !inDev && inUser) mode = 'User';

			// A link straight to a TrackNTrap or Gringo page selects that project
			const project = projectOf(path);
			if (project && project !== docProject.current) docProject.current = project;
		});
	});

	function switchMode(t) {
		if (t === mode) return;
		mode = t;
		goto('/');
	}

	function switchProject(project) {
		if (project === docProject.current) return;
		docProject.current = project;
		goto('/');
	}

	const sidebar = $derived(sidebarFor(docProject.current, mode));
	const sections = $derived(sidebar.sections);
	const pages = $derived(sidebar.pages);
</script>

<nav
	aria-label="Documentation"
	class="flex h-screen w-72 shrink-0 flex-col overflow-y-auto border-r border-[#1e2b26] bg-[#0a0e0c] py-7"
>
	<div
		class="mb-4 grid grid-cols-3 gap-1 border-b border-[#1e2b26] px-3 pb-5 font-mono text-xs"
		role="group"
		aria-label="Project"
	>
		{#each projects as project (project)}
			<button
				type="button"
				aria-pressed={docProject.current === project}
				onclick={() => switchProject(project)}
				class="cursor-pointer rounded border px-1 py-2 text-center transition-colors
					{docProject.current === project
						? 'border-accent/60 bg-accent/15 text-accent'
						: 'border-[#1e2b26] text-[#7f9189] hover:border-[#d8e4de]/40 hover:text-[#d8e4de]'}"
			>
				{project}
			</button>
		{/each}
	</div>

	<div class="px-6 pb-2">
		<div
			class="relative grid grid-cols-2 rounded-md border border-[#1e2b26] bg-[#0d1210] p-0.5 font-mono text-xs"
			role="group"
			aria-label="Documentation part"
		>
			<span
				class="absolute inset-y-0.5 left-0.5 w-[calc(50%-2px)] rounded bg-accent/15 transition-transform duration-200
					{mode === 'Developer' ? 'translate-x-full' : 'translate-x-0'}"
				aria-hidden="true"
			></span>
			{#each types as t (t)}
				<button
					type="button"
					aria-pressed={mode === t}
					onclick={() => switchMode(t)}
					class="relative z-10 cursor-pointer py-1.5 text-center transition-colors
						{mode === t ? 'text-accent' : 'text-[#7f9189] hover:text-[#d8e4de]'}"
				>
					{t}
				</button>
			{/each}
		</div>
	</div>

	{#each sections as section, n (section.title)}
		{@const sectionPages = pages.filter((p) => p.section === n)}

		<div class="flex flex-col">
			<h2 class="mt-5 px-6 py-1 font-mono text-lg font-bold text-[#d8e4de]">
				{section.title}
			</h2>

			<ul>
				{#each sectionPages as p, j (p.title)}
					{@const active = current.url.pathname === p.path}
					<li>
						<a
							href={p.path}
							aria-current={active ? 'page' : undefined}
							class="flex w-full items-center justify-between border-l-2 px-6 py-[7px] text-left text-sm transition-colors
								{active
								? 'border-accent bg-accent/[0.06] text-accent'
								: 'border-transparent text-[#7f9189] hover:text-[#d8e4de]'}"
						>
							<span>{p.title}</span>
							{#if active}
								<span class="font-mono text-xs" aria-hidden="true">{n + 1}.{j + 1}</span>
							{/if}
						</a>
					</li>
				{/each}
			</ul>
		</div>
	{/each}
</nav>
