<script>
	const systemExample = `class System {
  public:
    virtual ~System() = default;
    std::set<Entity> _entities;
};`;

	const matchExample = `(entity ComponentSet & required ComponentSet) == required ComponentSet`;

	const tableExample = `system           requires                     player   decoration
InputSystem      PlayerControlled + Velocity   yes      no
MovementSystem   Position + Velocity           yes      yes
RenderSystem     Position + Sprite             yes      yes`;

	const setupExample = `_movementSystem = _scene.registerSystem<MovementSystem>();

ComponentSet movementRequirements;
movementRequirements.set(_scene.getComponentType<Position>());
movementRequirements.set(_scene.getComponentType<Velocity>());
_scene.setSystemComponentSet<MovementSystem>(movementRequirements);`;

	const movementExample = `void MovementSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Position &position = scene.getComponent<Position>(entity);
        Velocity const &velocity = scene.getComponent<Velocity>(entity);

        position.x += velocity.dx * FIXED_DELTA_TIME;
        position.y += velocity.dy * FIXED_DELTA_TIME;
    }
}`;

	const newHeaderExample = `// src/systems/HealthSystem.hpp
#pragma once
#include "Scene.hpp"
#include "System.hpp"

class HealthSystem : public System
{
  public:
    void update(Scene &scene);
};`;

	const newSourceExample = `// src/systems/HealthSystem.cpp
#include "HealthSystem.hpp"
#include "Health.hpp"
#include <vector>

void HealthSystem::update(Scene &scene)
{
    std::vector<Entity> dead;

    for (Entity entity : _entities) {
        if (scene.getComponent<Health>(entity).current <= 0)
            dead.push_back(entity);
    }
    for (Entity entity : dead)
        scene.destroyEntity(entity);
}`;

	const newCmakeExample = `add_executable(\${PROJECT_NAME}
    ...
    src/systems/MovementSystem.cpp
    src/systems/HealthSystem.cpp   # new file
)`;

	const newAppExample = `// Application.hpp
std::shared_ptr<HealthSystem> _healthSystem;

// Application::setupSystems()
_healthSystem = _scene.registerSystem<HealthSystem>();
ComponentSet healthRequirements;
healthRequirements.set(_scene.getComponentType<Health>());
_scene.setSystemComponentSet<HealthSystem>(healthRequirements);

// Application::run(), in the fixed step loop
_healthSystem->update(_scene);`;
</script>

<svelte:head>
	<title>How systems work - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How systems work</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	How systems find their entities, what the R-Type systems do and how to add a new one
</p>

<p class="max-w-[100ch] leading-7">
	A system holds the logic of the game. It declares which components it needs, and the engine keeps
	for it the list of entities that have all of them. When the system runs, it only goes through this
	list.
</p>

<h2 class="mt-9 text-lg font-semibold">The System base class</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Every system inherits from {@render c('System')} ({@render c('src/engine/systems/System.hpp')}),
	which only holds the set of entities the system works on. The engine fills this set; a system only
	reads it.
</p>
{@render code(systemExample)}

<h2 class="mt-9 text-lg font-semibold">Required components</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The components a system needs are given as a {@render c('ComponentSet')}, with one bit per required
	component type. An entity belongs to the system when its own {@render c('ComponentSet')} contains
	every required bit. It may have more components, they are ignored:
</p>
{@render code(matchExample)}
<p class="mt-3 max-w-[100ch] leading-7">With the entities created by the game:</p>
{@render code(tableExample)}

<h2 class="mt-9 text-lg font-semibold">The SystemManager</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('SystemManager')} ({@render c('src/engine/systems/SystemManager.hpp')}) stores each
	system with its required {@render c('ComponentSet')}, keyed by the C++ type of the system. Through
	the {@render c('Scene')}:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('registerSystem<T>()')} creates the system and returns a
		{@render c('std::shared_ptr<T>')} to it, which the game keeps to call it.
	</li>
	<li>{@render c('setSystemComponentSet<T>(set)')} sets the components the system requires.</li>
	<li>
		Each time an entity's components change, every system compares the new
		{@render c('ComponentSet')} with its requirements and adds or removes the entity from its set.
	</li>
	<li>When an entity is destroyed, it is removed from every system.</li>
</ul>
{@render code(setupExample)}

<h2 class="mt-9 text-lg font-semibold">Running a system</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	There is no common {@render c('update')} method in {@render c('System')}: each system writes its
	own {@render c('update')} with the parameters it needs, and {@render c('Application')} calls them in
	order. A system loops over {@render c('_entities')} and reads its components from the
	{@render c('Scene')}:
</p>
{@render code(movementExample)}

<h2 class="mt-9 text-lg font-semibold">The R-Type systems</h2>
<p class="mt-3 max-w-[100ch] leading-7">They are in {@render c('src/systems')}:</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">InputSystem</strong> ({@render c('PlayerControlled')} +
		{@render c('Velocity')}): reads the arrow keys with raylib and sets the velocity to
		{@render c('PLAYER_SPEED')} (150 pixels per second) in the pressed direction, or to 0 when no key
		is pressed.
	</li>
	<li>
		<strong class="font-semibold">MovementSystem</strong> ({@render c('Position')} +
		{@render c('Velocity')}): moves each entity by its velocity multiplied by
		{@render c('FIXED_DELTA_TIME')}.
	</li>
	<li>
		<strong class="font-semibold">RenderSystem</strong> ({@render c('Position')} +
		{@render c('Sprite')}): draws each entity as a rectangle at its position. It receives the
		renderer as a parameter, so it never calls raylib directly.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Adding a system</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	This example removes the entities whose {@render c('Health')} drops to 0, using the component from
	<a href="/developer/ecs-components" class="text-[#ff8a3d] hover:underline">How components work</a>.
</p>
<p class="mt-3 max-w-[100ch] leading-7">1. Create the header and the source in {@render c('src/systems')}:</p>
{@render code(newHeaderExample)}
{@render code(newSourceExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The entities are collected first and destroyed after the loop, because destroying an entity removes
	it from {@render c('_entities')} while the loop is going through it.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	2. Add the source file to the client target in {@render c('CMakeLists.txt')}:
</p>
{@render code(newCmakeExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	3. Register the system, set its requirements and call it in {@render c('Application')}:
</p>
{@render code(newAppExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Set the requirements right after {@render c('registerSystem')}, before creating entities:
		entities that already have their components are not added afterwards.
	</li>
	<li>
		Registering the same system twice prints a message and returns {@render c('nullptr')}.
	</li>
	<li>
		{@render c('_entities')} is a {@render c('std::set')}, so entities are always processed in
		increasing id order.
	</li>
	<li>
		With {@render c('InputSystem')}, when two opposite keys are pressed, left wins over right and
		down wins over up.
	</li>
</ul>
