<script>
	// A short note under a command that runs a .sh script: those are Bash scripts, Linux only.
	// Usage: <LinuxOnly /> or, to replace the Windows sentence,
	// <LinuxOnly>On Windows, run the unit tests instead.</LinuxOnly>
	let { children } = $props();
</script>

<p class="mt-2 max-w-[100ch] text-sm leading-6 text-[#e8a33d]">
	<strong>Linux only:</strong> <code>.sh</code> files are Bash scripts and do not run on Windows.
	{#if children}
		{@render children()}
	{:else}
		On Windows, use the commands of the WINDOWS section instead.
	{/if}
</p>
