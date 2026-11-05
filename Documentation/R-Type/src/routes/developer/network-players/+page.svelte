<script>
	const scriptExample = `void PlayerScript::onUpdate(Scene &scene, Entity &entity)
{
    ActionInput const *input = nullptr;
    if (scene.hasComponent<RemoteInput>(entity))      // server: this player's client
        input = &scene.getComponent<RemoteInput>(entity).input;
    else if (scene.hasResource<ActionInput>())        // solo: local keys
        input = &scene.getResource<ActionInput>();
    if (input && input->isActive(0))
        /* move up */;
}`;
</script>

<svelte:head>
	<title>How players and inputs work - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How players and inputs work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Player slots, action input and disconnects</p>

<h2 class="mt-9 text-lg font-semibold">Players</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Each client that connects gets the lowest free slot {@render c('s')}; the server instantiates
		{@render c('PlayerPrefabs[s % count]')}, owned by that client. At most 4 clients.
	</li>
	<li>
		The prefab needs no {@render c('Networked')}: the server adds one ({@render c('Replicated')},
		{@render c('Prefab')} = its name). A {@render c('Server')} or {@render c('Client')} mode is warned
		once and never reaches the clients.
	</li>
	<li>
		A client finds its own mirror with the {@render c('LocalPeer')} resource: its
		{@render c('Networked.owner')} equals {@render c('LocalPeer::id')}.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Input</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Games read actions (0 to 31), not keys. {@render c('InputBindings')} maps keys to actions and
		{@render c('KeyboardSystem')} fills the {@render c('ActionInput')} resource, solo games included.
	</li>
	<li>A client sends its {@render c('ActionInput')} once per tick ({@render c('InputState')}).</li>
	<li>
		The server writes each client's latest input into the {@render c('RemoteInput')} component of
		every entity that client owns.
	</li>
</ul>
{@render code(scriptExample)}

<h2 class="mt-9 text-lg font-semibold">Disconnects</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	When a client leaves or times out, the server destroys its player, frees its slot,
	clears its input and sends {@render c('PeerLeft')} to the others ({@render c('Events<PeerLeft>')},
	{@render c('Timeout')} for a crash). The player's mirror goes away through the next
	{@render c('Despawn')}. A client that loses the server destroys every mirror.
</p>
