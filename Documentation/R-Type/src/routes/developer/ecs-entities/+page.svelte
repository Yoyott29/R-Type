<script>
	const typesExample = `#define MAX_ENTITIES 4096
#define MAX_COMPONENTS 32

using Entity = std::uint32_t;
using ComponentType = std::uint8_t;
using ComponentSet = std::bitset<MAX_COMPONENTS>;`;

	const managerExample = `class EntityManager {
  public:
    Entity createEntity();
    void destroyEntity(Entity entity);
    void setupComponentSet(Entity entity, ComponentSet componentSet);
    ComponentSet getComponentSet(Entity entity) const;
    bool isAlive(Entity entity) const;

  private:
    std::queue<Entity> _availableEntities;                 // free ids
    std::array<ComponentSet, MAX_ENTITIES> _componentSets; // components of each entity
    std::uint32_t _livingEntityCount;
    std::bitset<MAX_ENTITIES> _alive;                       // one bit per id
};`;

	const queueExample = `start               free ids: 0 1 2 3 ... 4095
createEntity() -> 0  free ids: 1 2 3 ... 4095
createEntity() -> 1  free ids: 2 3 ... 4095
destroyEntity(0)     free ids: 2 3 ... 4095 0`;

	const setExample = `registered types:  Position = 0, Velocity = 1, Sprite = 2, PlayerControlled = 3

player     Position + Velocity + Sprite + PlayerControlled   ->  ...0000 1111
decoration Position + Velocity + Sprite                      ->  ...0000 0111`;
</script>

<svelte:head>
	<title>How entities work - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How entities work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How entity ids are created, recycled and linked to their components
</p>

<p class="max-w-[100ch] leading-7">
	An entity is only a number. It does not store any data and has no methods: its data lives in the
	component pools, indexed by this number. Entities are handled by the {@render c('EntityManager')}
	({@render c('src/engine/core/EntityManager.hpp')}), through the {@render c('Scene')}.
</p>

<h2 class="mt-9 text-lg font-semibold">The base types</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('src/engine/core/Entity.hpp')} defines the types and limits used by the whole engine:
</p>
{@render code(typesExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('Entity')}: the id of an entity, from {@render c('0')} to
		{@render c('MAX_ENTITIES - 1')}.
	</li>
	<li>
		{@render c('ComponentType')}: the id of a component type, given when the type is registered.
	</li>
	<li>
		{@render c('ComponentSet')}: one bit per component type, set when the entity has this component.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">The EntityManager</h2>
{@render code(managerExample)}

<h3 class="mt-6 font-semibold">Creating and destroying</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	At construction, every id from {@render c('0')} to {@render c('4095')} is pushed into a queue of
	free ids. {@render c('createEntity')} takes the id at the front of the queue and marks it alive.
	{@render c('destroyEntity')} clears the entity's {@render c('ComponentSet')}, marks it dead and
	pushes its id at the back of the queue. A destroyed id is therefore reused as late as possible.
</p>
{@render code(queueExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Creating an entity while 4096 are already alive throws {@render c('EntityLimitError')}.
</p>

<h3 class="mt-6 font-semibold">The ComponentSet of each entity</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	The manager keeps one {@render c('ComponentSet')} per entity. The {@render c('Scene')} updates it
	each time a component is added or removed, and the systems compare it with their requirements to
	know if the entity concerns them. The bit used by a component is its {@render c('ComponentType')},
	which follows the registration order:
</p>
{@render code(setExample)}

<h3 class="mt-6 font-semibold">Checking an entity</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('isAlive')} tells if an id is currently used. {@render c('Scene::destroyEntity')} uses it
	to ignore entities that are already destroyed. {@render c('getComponentSet')} throws
	{@render c('OutOfRangeEntityError')} for an id greater than or equal to {@render c('MAX_ENTITIES')}.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Always create and destroy entities through the {@render c('Scene')}, never through the
		{@render c('EntityManager')} directly: the Scene also cleans the component pools and the systems.
	</li>
	<li>
		An {@render c('Entity')} value is only valid while the entity is alive. There is no version
		number in the id, so an id kept after {@render c('destroyEntity')} can later point to a new
		entity.
	</li>
	<li>
		The arrays indexed by entity id are allocated once, with {@render c('MAX_ENTITIES')} entries, when
		the manager is created.
	</li>
</ul>
