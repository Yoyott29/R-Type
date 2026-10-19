<script>
	const serverExample = `each tick, after the systems ran
  buildSnapshot()   -> one 25 byte record per alive entity in _entities
                       (players, enemies, bullets, stars):
                       position, type, player id, variant, size, shape,
                       draw order and color
  sendSnapshot()    -> keeps it in _snapshotHistory[tick % 64]
                    -> cuts it in packets of at most 40 entities, all with the same tick
                    -> sends every packet to every connected player`;

	const splitExample = `(1024 - 7 header bytes) / 25 bytes per entity = 40 entities per packet

100 entities at tick 42
  [SNAPSHOT, 42, count 40, ...]
  [SNAPSHOT, 42, count 40, ...]
  [SNAPSHOT, 42, count 20, ...]`;

	const clientExample = `each frame, ReadSocket::update
  read every datagram waiting on the socket
    SNAPSHOT older than the one on screen    -> ignored
    SNAPSHOT newer than the parts kept       -> forget the parts, keep this one
    SNAPSHOT of the same tick                -> keep it too
  applySnapshot()
    no snapshot this frame                   -> keep the entities on screen
    newer tick than the one on screen        -> destroy every entity, rebuild
    same tick (a late part)                  -> add its entities`;
</script>

<svelte:head>
	<title>How snapshots work - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How snapshots work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How the server sends the game state and how the client shows it
</p>

<p class="max-w-[100ch] leading-7">
	The server is the only one that runs the game. 60 times per second, it sends a
	{@render c('SNAPSHOT')}: the position, the type and the look (size, shape, color, draw order) of
	every entity. The exact byte layout is on
	<a href="/developer/network-protocol" class="text-[#ff8a3d] hover:underline">How the protocol works</a>.
	The client does not simulate
	anything, it only draws the last snapshot it received.
</p>

<h2 class="mt-9 text-lg font-semibold">On the server</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('GameLoop::buildSnapshot')} goes through {@render c('_entities')}, the players,
	enemies, bullets and stars the game loop created. The variant is
	{@render c('ENEMY_VARIANT_2')} when the sprite of an enemy is {@render c('Assets/Bydos2.png')}, and
	the player id is {@render c('NO_PLAYER')} (0xFF) for anything but a player. {@render c('Network::sendSnapshot')} copies the list
	of players under the mutex, then sends without holding it.
</p>
{@render code(serverExample)}

<h2 class="mt-9 text-lg font-semibold">Splitting a snapshot</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A packet holds at most 1024 bytes. When the entities do not fit, the snapshot is sent in several
	packets with the same tick. The {@render c('count')} of each packet is the number of entities in
	that packet, not in the whole snapshot.
</p>
{@render code(splitExample)}

<h2 class="mt-9 text-lg font-semibold">On the client</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('ReadSocket')} reads every datagram waiting on the socket, but only applies the newest
	tick. After a freeze (a window dragged, a breakpoint), hundreds of snapshots can be waiting:
	applying only the newest one avoids drawing old frames and running out of entities.
</p>
{@render code(clientExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Each new entity gets a {@render c('Position')}, a {@render c('Type')} and a
	{@render c('Sprite')} built from the shape, size, color and draw order of the snapshot. The texture
	is picked on the client: from the player id for a player, from the type and the variant for the
	others (see {@render c('texturePathForType')} in {@render c('ReadSocket.cpp')}). The tick of the snapshot is stored in
	{@render c('Socket::lastSnapshotTick')} and sent back in each {@render c('INPUT')}.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The server and the clients have separate scenes, so their entity ids do not match. The client
		rebuilds its entities from scratch on each new tick. A network id will be needed to update
		entities instead of rebuilding them.
	</li>
	<li>
		The snapshot history ({@render c('_snapshotHistory')}) is kept for the future
		{@render c('DELTA')} packets, it is not read yet.
	</li>
	<li>
		A snapshot with an unknown entity type (above 4), or a player with an id above 3, throws
		{@render c('UnknownComponentType')} on the client. Nothing catches it before
		{@render c('main')}, so the client stops with {@render c('Fatal error: ...')}.
	</li>
	<li>
		The server never sends an empty snapshot: with no entity, no packet is sent. In practice the
		stars are always there.
	</li>
</ul>
