<script>
	import { page as current } from '$app/state';
	import { goto } from '$app/navigation';

	let { sections = [], pages = [] } = $props();
</script>

<div
	class="flex h-screen w-60 shrink-0 flex-col overflow-y-auto border-r border-[#1e2b26] bg-[#0a0e0c] py-7 [scrollbar-width:none] [&::-webkit-scrollbar]:hidden"
>
	<div class="mb-4 border-b border-[#1e2b26] px-6 pb-5 font-mono text-[13px] text-[#7f9189]">
		<strong class="font-semibold text-[#d8e4de]">R-Type</strong> / Documentation
	</div>

	{#each sections as section, i (section.title)}
		{@const sectionPages = pages.filter((p) => p.section === i)}

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
						<span class="font-mono text-xs">{i + 1}.{j + 1}</span>
					{/if}
				</button>
			{/each}
		</div>
	{/each}
</div>