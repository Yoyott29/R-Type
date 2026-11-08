<script>
	// A big boxed section holding the instructions for one operating system.
	// Usage: <Platform os="windows">...</Platform> or <Platform os="linux">...</Platform>
	let { os, children } = $props();

	const windows = os === 'windows';
</script>

<section
	class="mt-5 max-w-[100ch] rounded-md border border-l-4 border-[#1e2b26] px-5 pt-4 pb-5 {windows
		? 'border-l-[#4c8bf5]'
		: 'border-l-[#e8a33d]'}"
>
	<h3
		class="text-base font-bold tracking-[0.18em] {windows ? 'text-[#4c8bf5]' : 'text-[#e8a33d]'}"
	>
		{windows ? 'WINDOWS' : 'LINUX'}
	</h3>
	{@render children()}
</section>
