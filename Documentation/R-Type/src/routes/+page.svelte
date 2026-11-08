<script>
	import RTypeLogo from '$lib/components/RTypeLogo.svelte';
    import { docMode } from '$lib/mode.svelte.js';
	import { docProject } from '$lib/project.svelte.js';

	// What the home page shows for each project: a one line description and its cards
	const projects = {
		'R-Type': {
			text: "A side-scrolling shoot 'em up built with a custom ECS engine, an authoritative server and networked multiplayer clients, in C++20 with raylib.",
			cards: [
				{
					title: 'User',
					text: 'Install the game, launch it and learn how to play.',
					href: '/user/getting-started',
					cta: 'Getting started'
				},
				{
					title: 'Developer',
					text: 'Architecture, workflows, tooling and conventions of the project.',
					href: '/developer/about',
					cta: 'Read the dev docs'
				}
			]
		},
		TrackNTrap: {
			text: 'A 3D multiplayer race between rats on a kitchen track full of knives, spatulas and brooms, made with the Gringo Engine.',
			cards: [
				{
					title: 'User',
					text: 'Build the game, launch a race and learn how to play.',
					href: '/trackntrap/getting-started',
					cta: 'Getting started'
				},
				{
					title: 'Developer',
					text: 'How the game is built, what each script does and how to modify it.',
					href: '/trackntrap/developer/architecture',
					cta: 'Read the dev docs'
				}
			]
		},
		Gringo: {
			text: 'The ECS game engine behind R-Type and TrackNTrap, with an editor to create, edit and build your own games.',
			cards: [
				{
					title: 'User',
					text: 'Build the engine, open the editor and make a game with it.',
					href: '/gringo/getting-started',
					cta: 'Getting started'
				},
				{
					title: 'Developer',
					text: 'How the engine works inside: ECS, plugins, network and editor.',
					href: '/gringo/developer/architecture',
					cta: 'Read the dev docs'
				}
			]
		}
	};

	const project = $derived(projects[docProject.current]);

	// star pattern shared by the two parallax layers
	const starPattern =
		'bg-[image:radial-gradient(1px_1px_at_20px_30px,#d8e4de,transparent),radial-gradient(1px_1px_at_90px_120px,#d8e4de,transparent),radial-gradient(1.5px_1.5px_at_160px_60px,var(--accent),transparent),radial-gradient(1px_1px_at_40px_160px,#7f9189,transparent),radial-gradient(1px_1px_at_130px_180px,#d8e4de,transparent)] bg-[length:var(--tile)_var(--tile)]';
</script>

<svelte:head>
<title>{docProject.current} docs</title>
</svelte:head>

<section
	class="relative isolate overflow-hidden rounded-xl border border-[#1e2b26] bg-[#0a0e0c] px-6 pt-20 pb-28 text-center sm:pt-28 sm:pb-36"
>
	<!-- animated background -->
	<div
		class="absolute inset-0 -z-10 opacity-70 [--tile:200px] motion-safe:animate-drift {starPattern}"
		aria-hidden="true"
	></div>
	<div
		class="absolute inset-0 -z-10 bg-position-[60px_90px] opacity-35 [--tile:320px] motion-safe:animate-drift-slow {starPattern}"
		aria-hidden="true"
	></div>
	<div
		class="absolute inset-0 -z-10 bg-[image:radial-gradient(ellipse_60%_45%_at_50%_40%,rgb(63_190_214/0.14),transparent_70%),radial-gradient(ellipse_70%_35%_at_50%_110%,color-mix(in_oklab,var(--accent)_18%,transparent),transparent_70%)]"
		aria-hidden="true"
	></div>
	<div
		class="pointer-events-none absolute inset-0 -z-10 bg-[image:repeating-linear-gradient(to_bottom,transparent_0,transparent_2px,rgb(0_0_0/0.18)_3px)]"
		aria-hidden="true"
	></div>

	<!-- shooting beam: travels along a thin rail at the bottom, under the text -->
	<div
		class="absolute bottom-16 left-0 -z-10 h-0.5 w-[30%] -translate-x-full bg-linear-to-r from-transparent via-accent to-white opacity-0 drop-shadow-[0_0_6px_var(--accent)] motion-safe:animate-beam"
		aria-hidden="true"
	></div>

	<!-- content -->
	<div class="relative flex flex-col items-center">
		<p class="mb-6 font-mono text-xs tracking-[0.4em] text-accent uppercase">
			Epitech project
		</p>

		{#if docProject.current === 'R-Type'}
			<RTypeLogo
				class="w-full max-w-xl drop-shadow-[0_0_18px_rgb(63_190_214/0.35)] motion-safe:animate-float"
			/>
		{:else}
			<h1 class="font-mono text-5xl font-bold tracking-[0.2em] text-[#d8e4de]">
				{docProject.current}
			</h1>
		{/if}

		<div class="mt-8 flex w-full max-w-3xl items-center gap-4">
			<span class="h-px flex-1 bg-gradient-to-r from-transparent to-accent/60"></span>
			<h1
				class="text-center font-mono text-lg font-semibold tracking-[0.3em] text-[#d8e4de] sm:text-2xl sm:tracking-[0.4em]"
			>
				<span class="text-accent">{docMode.current.toUpperCase()}</span> DOCUMENTATION
			</h1>
			<span class="h-px flex-1 bg-gradient-to-l from-transparent to-accent/60"></span>
		</div>

		<p class="mt-6 max-w-[58ch] leading-7 text-[#7f9189]">{project.text}</p>
	</div>
</section>

<div class="mt-8 grid gap-4 sm:grid-cols-2">
	{#each project.cards as card (card.title)}
		<a
			href={card.href}
			class="block rounded-xl border border-[#1e2b26] bg-[#0a0e0c] p-6 transition-colors hover:border-accent/60"
		>
			<h2 class="font-mono text-lg font-semibold text-[#d8e4de]">{card.title}</h2>
			<p class="mt-2 leading-7 text-[#7f9189]">{card.text}</p>
			<span class="mt-4 inline-block font-mono text-sm text-accent">{card.cta} →</span>
		</a>
	{/each}
</div>
