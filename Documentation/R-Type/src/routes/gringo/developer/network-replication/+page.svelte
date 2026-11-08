<script>
	const codecExample = `#include "Replication.hpp"

struct Health { int value; };

template <> struct NetCodec<Health> {
    static void write(ByteWriter &w, Health const &h) { w.write<std::int32_t>(h.value); }
    static Health read(ByteReader &r) { return Health{r.read<std::int32_t>()}; }
};

// onStart of a script on an entity WITHOUT Networked, so it runs on both sides
replicate<Health>(scene, "Health");`;
</script>

<svelte:head>
	<title>How replication works - Gringo docs</title>
</svelte:head>

{#snippet code(text)}
	<pre
		class="mt-3 overflow-x-auto rounded-md border border-[#1e2b26] bg-[#121a17] p-4 font-mono text-[13.5px] leading-6"><code
			>{text}</code
		></pre>
{/snippet}

{#snippet c(text)}<code
		class="rounded border border-[#1e2b26] bg-[#121a17] px-1.5 py-0.5 font-mono text-[13.5px]"
		>{text}</code
	>{/snippet}

<h1 class="text-3xl font-semibold">How replication works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How server entities become client mirrors, with no network code in the game
</p>

<h2 class="mt-9 text-lg font-semibold">On the server</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('ReplicationSystem')} gives each {@render c('Replicated')} entity a {@render c('NetId')}
		(from 1, never reused).
	</li>
	<li>
		Whenever {@render c('tick / 2')} changes (30 per second), it sends one {@render c('EntityState')}
		per entity to every client, with the full state of every replicated component.
	</li>
	<li>
		An entity that left the snapshot is sent in {@render c('Despawn')}, repeated in 3 snapshots.
	</li>
	<li>An entity too big for one datagram is skipped and logged once per prefab; the others go out.</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">On the client</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('MirrorSystem')} keys mirrors by {@render c('netId')}. An unknown id instantiates
		{@render c('prefab')} (unknown prefab: dropped, logged once); a known one gets its components
		overwritten, and the replicated components missing from the message are removed.
	</li>
	<li>Messages older than the highest tick seen are ignored.</li>
	<li>
		A mirror not updated for 30 ticks is destroyed. At most 4096 mirrors. A mirror destroyed by a
		script comes back with the next snapshot.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Replicating a component</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('Transform')} is built in. For another component, write a {@render c('NetCodec')} and
	register it under the same name on both sides; {@render c('ChildOf')} is never replicated.
</p>
{@render code(codecExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Mirrors also run their prefab script's {@render c('onUpdate')}: guard authority logic with
		{@render c('BuildVariantName')} ({@render c('"server"')}).
	</li>
	<li>
		The server has no renderer plugin, so its entities never have renderer components. Replicating
		one (a Sprite) removes it from every mirror.
	</li>
	<li>No interpolation and no prediction: mirrors jump from snapshot to snapshot.</li>
</ul>
