<script>
	import RTypeLogo from '$lib/components/RTypeLogo.svelte';
    import { docMode } from '$lib/mode.svelte.js';

	const cards = [
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
	];

	// star pattern shared by the two parallax layers
	const starPattern =
		'bg-[image:radial-gradient(1px_1px_at_20px_30px,#d8e4de,transparent),radial-gradient(1px_1px_at_90px_120px,#d8e4de,transparent),radial-gradient(1.5px_1.5px_at_160px_60px,#ff8a3d,transparent),radial-gradient(1px_1px_at_40px_160px,#7f9189,transparent),radial-gradient(1px_1px_at_130px_180px,#d8e4de,transparent)] bg-[length:var(--tile)_var(--tile)]';
</script>

<svelte:head>
	<title>R-Type docs</title>
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
		class="absolute inset-0 -z-10 bg-[image:radial-gradient(ellipse_60%_45%_at_50%_40%,rgb(63_190_214/0.14),transparent_70%),radial-gradient(ellipse_70%_35%_at_50%_110%,rgb(255_138_61/0.18),transparent_70%)]"
		aria-hidden="true"
	></div>
	<div
		class="pointer-events-none absolute inset-0 -z-10 bg-[image:repeating-linear-gradient(to_bottom,transparent_0,transparent_2px,rgb(0_0_0/0.18)_3px)]"
		aria-hidden="true"
	></div>

	<!-- shooting beam: travels along a thin rail at the bottom, under the text -->
	<div
		class="absolute bottom-16 left-0 -z-10 h-0.5 w-[30%] -translate-x-full bg-linear-to-r from-transparent via-[#ff8a3d] to-white opacity-0 drop-shadow-[0_0_6px_#ff8a3d] motion-safe:animate-beam"
		aria-hidden="true"
	></div>

	<!-- content -->
	<div class="relative flex flex-col items-center">
		<p class="mb-6 font-mono text-xs tracking-[0.4em] text-[#ff8a3d] uppercase">
			Epitech project
		</p>

		<RTypeLogo
			class="w-full max-w-xl drop-shadow-[0_0_18px_rgb(63_190_214/0.35)] motion-safe:animate-float"
		/>

		<div class="mt-8 flex w-full max-w-3xl items-center gap-4">
			<span class="h-px flex-1 bg-gradient-to-r from-transparent to-[#ff8a3d]/60"></span>
			<h1
				class="text-center font-mono text-lg font-semibold tracking-[0.3em] text-[#d8e4de] sm:text-2xl sm:tracking-[0.4em]"
			>
				<span class="text-[#ff8a3d]">{docMode.current.toUpperCase()}</span> DOCUMENTATION
			</h1>
			<span class="h-px flex-1 bg-gradient-to-l from-transparent to-[#ff8a3d]/60"></span>
		</div>

		<p class="mt-6 max-w-[58ch] leading-7 text-[#7f9189]">
			A side-scrolling shoot 'em up built with a custom ECS engine, an authoritative server and
			networked multiplayer clients, in C++17 with raylib.
		</p>
	</div>
</section>
