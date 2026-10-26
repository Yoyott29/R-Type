<script>
	const serverExample = `SnapshotSystem::update (Update, once per new ServerTick)
  buildEntity()     -> one 11 byte record per entity with Position + Type:
                       (players, enemies, bullets, stars)
                       position, type, player id, variant
  encodeSnapshot()  -> cuts it in messages of at most 107 entities, all with the same tick
  broadcast()       -> queues every message for every connected player,
                       sent by the NetworkPlugin in PostUpdate`;

	const splitExample = `(1191 payload bytes - 6 header bytes) / 11 bytes per entity = 107 entities per message

250 entities at tick 42
  [Snapshot, 42, count 107, ...]
  [Snapshot, 42, count 107, ...]
  [Snapshot, 42, count 36, ...]`;

	const clientExample = `ClientNetSystem::update (PrepUpdate, every frame)
  read every NetMessage of this frame
    Snapshot          -> decodeSnapshot(), kept in a list (malformed: dropped)
  applySnapshots()
    no snapshot this frame                   -> keep the entities on screen
    newest tick older than the one on screen -> ignored
    newer tick than the one on screen        -> destroy every entity, rebuild
    same tick (a late part)                  -> add its entities`;

	const lookExample = `type      variant / playerId      Sprite (client only)
PLAYER    playerId 0..3           box 103 x 50, Assets/Player{Green,Blue,Magenta,Red}.png
ENEMY     DEFAULT_VARIANT         box, Assets/Bydos1.png
ENEMY     ENEMY_VARIANT_2         box, Assets/Bydos2.png
BULLET    -                       box 20 x 10, Assets/Bullet.png
OTHER     -                       white box 5 x 5, drawOrder -1 (stars, behind everything)
OBSTACLE  -                       grey box 32 x 32 (no obstacle is spawned yet)`;
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
	The server is the only one that runs the game. At each new tick (60 per second), it sends a
	{@render c('Snapshot')}: the position, the type, the variant and the player id of every entity.
	It never sends how an entity looks: the server has no {@render c('Sprite')}, and the client picks
	the look from the type. The exact byte layout is on
	<a href="/developer/network-protocol" class="text-[#ff8a3d] hover:underline">How the protocol works</a>.
	The client does not simulate anything, it only draws the last snapshot it received.
</p>

<h2 class="mt-9 text-lg font-semibold">On the server</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('SnapshotSystem')} ({@render c('src/games/rtype/server/systems')}) requires
	{@render c('Position')} + {@render c('Type')}, so it sees the players, enemies, bullets and stars.
	The variant comes from {@render c('Type::variant')}, set by the prefab
	({@render c('ENEMY_VARIANT_2')} for {@render c('"Enemy2"')}), and the player id is read from
	{@render c('PlayerId')} for a player, {@render c('NO_PLAYER')} (0xFF) for anything else.
</p>
{@render code(serverExample)}

<h2 class="mt-9 text-lg font-semibold">Splitting a snapshot</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A message payload holds at most 1191 bytes. When the entities do not fit, the snapshot is sent as
	several messages with the same tick. The {@render c('count')} of each message is the number of
	entities in that message, not in the whole snapshot. The NetworkPlugin then packs the messages
	into as few datagrams as it can.
</p>
{@render code(splitExample)}

<h2 class="mt-9 text-lg font-semibold">On the client</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('ClientNetSystem')} reads every snapshot of the frame, but only applies the newest
	tick. After a freeze (a window dragged, a breakpoint), many snapshots can be waiting: applying only
	the newest one avoids drawing old frames and running out of entities.
</p>
{@render code(clientExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Each new entity gets a {@render c('Position')}, a {@render c('Type')} (with its variant) and a
	{@render c('Sprite')} from {@render c('spriteForType')} in {@render c('ClientNetSystem.cpp')}. To
	change how something looks, only the client changes:
</p>
{@render code(lookExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The server and the clients have separate scenes, so their entity ids do not match. The client
		rebuilds every entity from scratch on each new tick, including any entity it created itself. A
		network id will be needed to update entities instead of rebuilding them.
	</li>
	<li>
		A malformed snapshot (size not matching its count, an entity type above 4, a player with an id
		above 3) throws {@render c('NetError')} in {@render c('decodeSnapshot')}. The client catches
		it and drops that message, the others are still applied.
	</li>
	<li>
		The server never sends an empty snapshot: with no entity, no message is sent. In practice the
		stars are always there.
	</li>
	<li>
		The size of a sprite on the client and the hit box ({@render c('Collider2D')}) on the server are
		set separately, so they can differ.
	</li>
</ul>
