<script>
	const componentsExample = `struct Position {
    float x;
    float y;
};

struct Velocity {
    float dx; // pixels per second
    float dy;
};

struct Sprite {
    float width;
    float height;
    ColorRGBA color;
};

struct PlayerControlled {
};`;

	const registerExample = `scene.registerComponent<Position>();         // ComponentType 0
scene.registerComponent<Velocity>();         // ComponentType 1
scene.registerComponent<Sprite>();           // ComponentType 2
scene.registerComponent<PlayerControlled>(); // ComponentType 3`;

	const useExample = `scene.addComponent(player, Position{0.0f, 200.0f});

Position &position = scene.getComponent<Position>(player);
position.x += 10.0f; // modifies the value stored in the pool

scene.removeComponent<Position>(player);`;

	const poolExample = `std::array<std::size_t, MAX_ENTITIES> _entityToPackedIndex; // entity -> index, or NULL_INDEX
std::vector<Component> _packedComponents;                    // the values, with no holes
std::vector<Entity> _packedIndexToEntity;                    // index -> entity`;

	const insertExample = `entities 3, 7 and 12 have a Position

_packedComponents     [ P3 | P7 | P12 ]
_packedIndexToEntity  [  3 |  7 |  12 ]
_entityToPackedIndex  3 -> 0, 7 -> 1, 12 -> 2, every other entity -> NULL_INDEX`;

	const removeExample = `removeComponent(3): the last value (P12) is moved into the hole

_packedComponents     [ P12 | P7 ]
_packedIndexToEntity  [  12 |  7 ]
_entityToPackedIndex  12 -> 0, 7 -> 1, 3 -> NULL_INDEX`;

	const newComponentExample = `// src/components/Health.hpp
#pragma once

struct Health {
    int current;
    int max;
};`;

	const newRegisterExample = `// src/game-rtype/Application.cpp
void Application::setupComponents()
{
    ...
    _scene.registerComponent<Health>();
}

void Application::setupEntities()
{
    ...
    _scene.addComponent(playerOne, Health{3, 3});
}`;
</script>

<svelte:head>
	<title>How components work - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How components work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	What a component is, how its values are stored and how to add a new one
</p>

<p class="max-w-[100ch] leading-7">
	A component is a plain struct that holds data and nothing else: no methods and no logic. An entity
	gets a behaviour by receiving components, and the systems read and modify these values.
</p>

<h2 class="mt-9 text-lg font-semibold">The R-Type components</h2>
<p class="mt-3 max-w-[100ch] leading-7">They are in {@render c('src/components')}:</p>
{@render code(componentsExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('Position')}: the position of the entity on screen, in pixels.</li>
	<li>{@render c('Velocity')}: the speed of the entity, in pixels per second.</li>
	<li>
		{@render c('Sprite')}: how the entity is drawn. For now it is a coloured rectangle of
		{@render c('width')} by {@render c('height')} pixels.
	</li>
	<li>
		{@render c('PlayerControlled')}: an empty component, used as a tag. It only marks the entities
		that the keyboard controls.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Registering a component</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each component type must be registered once, before it is used. Registering gives the type its
	{@render c('ComponentType')}, which is its bit in the entities' {@render c('ComponentSet')}, and
	creates the pool that stores its values. Ids are given in registration order:
</p>
{@render code(registerExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Registering the same type twice prints a message and does nothing. Using a type that is not
	registered throws {@render c('ComponentNotRegisteredError')}.
</p>

<h2 class="mt-9 text-lg font-semibold">Using components</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('getComponent')} returns a reference to the value stored in the pool, so modifying it
	modifies the entity. Asking for a component the entity does not have throws
	{@render c('MissingComponentError')}.
</p>
{@render code(useExample)}

<h2 class="mt-9 text-lg font-semibold">How values are stored: the ComponentPool</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each component type has its own {@render c('ComponentPool')}
	({@render c('src/engine/core/ComponentPool.hpp')}). It is a <strong class="font-semibold"
		>sparse set</strong
	>: the values are packed in one contiguous array, without holes, and two index arrays link entities
	and positions in that array.
</p>
{@render code(poolExample)}
{@render code(insertExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Finding the component of an entity is a direct array access, and a system that reads all the values
	goes through contiguous memory, which is fast for the CPU cache.
</p>

<h3 class="mt-6 font-semibold">Adding and removing</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	A new value is always appended at the end of the packed array. To remove a value without leaving a
	hole, the last value of the array is moved into its place and the index arrays are updated:
</p>
{@render code(removeExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Because of this, the order of the values in the pool changes over time and must not be relied on.
</p>

<h3 class="mt-6 font-semibold">The ComponentManager</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('ComponentManager')} ({@render c('src/engine/core/ComponentManager.hpp')}) owns every
	pool, in a map keyed by the C++ type of the component. Pools are stored through the non-template
	interface {@render c('IComponentPool')}, which lets the manager call
	{@render c('destroyEntity')} on every pool when an entity is destroyed, without knowing their types.
</p>

<h2 class="mt-9 text-lg font-semibold">Adding a component</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	1. Create a header in {@render c('src/components')} with a plain struct:
</p>
{@render code(newComponentExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	2. Register it in {@render c('Application::setupComponents')} and give it to the entities that need
	it:
</p>
{@render code(newRegisterExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	A component is a header only, so there is nothing to add to {@render c('CMakeLists.txt')}:
	{@render c('src/components')} is already in the include path. Then write or update a system to use
	it, see <a href="/developer/ecs-systems" class="text-[#ff8a3d] hover:underline">How systems work</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>The engine supports up to {@render c('MAX_COMPONENTS')} (32) component types.</li>
	<li>
		Adding a component an entity already has prints a message and keeps the old value. Use
		{@render c('getComponent')} to change it.
	</li>
	<li>
		{@render c('addComponent')} copies the value into the pool, so keep components small and simple.
	</li>
</ul>
