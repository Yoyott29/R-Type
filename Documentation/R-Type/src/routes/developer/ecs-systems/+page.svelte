<script>
	const systemExample = `class Scene;
class System
{
  public:
    virtual ~System() = default;
    virtual void update(Scene &scene) = 0;
    std::set<Entity> _entities;
};`;

	const matchExample = `(entity ComponentSet & required ComponentSet) == required ComponentSet`;

	const tableExample = `system            requires                 client entity   server player   server spawner
MovementSystem    Position + Velocity      no              yes             no
Collider2DSystem  Position + Collider2D    no              yes             no
ScriptSystem      Script                   no              yes             yes
RenderSystem      Position + Sprite        yes             -               -`;

	const setupExample = `// Physics::build(Application &app)
app._scene.registerSystem<MovementSystem>(FixedUpdate);
ComponentSet movementRequirements;
movementRequirements.set(app._scene.getComponentType<Position>());
movementRequirements.set(app._scene.getComponentType<Velocity>());
app._scene.setSystemComponentSet<MovementSystem>(movementRequirements);`;

	const labelExample = `enum ScheduleLabel { PrepUpdate, StateTransition, FixedUpdate, Update, PostUpdate };`;

	const runExample = `// Application::run()
while (_scene.getResource<AppAlive>().alive) {
    ...
    accumulator += frameTime;

    _scene.runSchedule(PrepUpdate);
    _scene.runSchedule(StateTransition);
    while (accumulator >= FIXED_DELTA_TIME) {
        _scene.runSchedule(FixedUpdate);
        _scene.flushDestroyQueue();
        accumulator -= FIXED_DELTA_TIME;
    }
    _scene.runSchedule(Update);
    _scene.runSchedule(PostUpdate);
}`;

	const movementExample = `void MovementSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Position &position = scene.getComponent<Position>(entity);
        Velocity const &velocity = scene.getComponent<Velocity>(entity);

        position.x += velocity.dx * FIXED_DELTA_TIME;
        position.y += velocity.dy * FIXED_DELTA_TIME;
    }
}`;

	const pluginExample = `class IPlugin
{
  public:
    virtual ~IPlugin() = default;
    virtual void build(Application &app) = 0;
    virtual void cleanup(Application &app) = 0;
};

// Application constructor
addPlugin(std::make_shared<RayLibRenderer>()); // GetInput, RenderSystem
addPlugin(std::make_shared<Physics>());        // MovementSystem, Collider2DSystem
addPlugin(std::make_shared<Client>(serverAddress, serverPort)); // TryConnect, SendInput, ReadSocket`;

	const newHeaderExample = `// src/systems/OffScreenSystem.hpp
#pragma once
#include "Position.hpp"
#include "Scene.hpp"
#include "System.hpp"

class OffScreenSystem : public System
{
  public:
    void update(Scene &scene) override;
};`;

	const newSourceExample = `// src/systems/OffScreenSystem.cpp
#include "OffScreenSystem.hpp"

void OffScreenSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        if (scene.getComponent<Position>(entity).x < -100.0f)
            scene.queueDestroy(entity);
    }
}`;

	const newCmakeExample = `add_executable(r-type_server
    ...
    src/systems/ScriptSystem.cpp
    src/systems/OffScreenSystem.cpp   # new file
    ...
)`;

	const newAppExample = `// GameLoop::GameLoop(...), src/server/game/game_loop.cpp
_scene.registerSystem<OffScreenSystem>(FixedUpdate);
ComponentSet offScreenRequirements;
offScreenRequirements.set(_scene.getComponentType<Position>());
_scene.setSystemComponentSet<OffScreenSystem>(offScreenRequirements);`;
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
	which holds the set of entities the system works on and declares the one method every system
	implements: {@render c('update(Scene &scene)')}. The engine fills the set; a system only reads it.
</p>
{@render code(systemExample)}

<h2 class="mt-9 text-lg font-semibold">Required components</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The components a system needs are given as a {@render c('ComponentSet')}, with one bit per required
	component type. An entity belongs to the system when its own {@render c('ComponentSet')} contains
	every required bit. It may have more components, they are ignored:
</p>
{@render code(matchExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	With the entities of the game (a client entity is created from a snapshot with
	{@render c('Position')} + {@render c('Type')} + {@render c('Sprite')}; the server player also has
	{@render c('Velocity')}, {@render c('Collider2D')}, {@render c('Script')} and
	{@render c('PlayerId')}; the two server spawners, one for enemies and one for stars, only have a
	{@render c('Script')}):
</p>
{@render code(tableExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	A system that never gets a {@render c('ComponentSet')} ({@render c('GetInput')},
	{@render c('TryConnect')}, {@render c('SendInput')}, {@render c('ReadSocket')}) keeps an empty
	{@render c('_entities')}: it works on {@render c('Scene')} resources instead.
</p>

<h2 class="mt-9 text-lg font-semibold">The SystemManager</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('SystemManager')} ({@render c('src/engine/systems/SystemManager.hpp')}) stores each
	system with its required {@render c('ComponentSet')}, keyed by the C++ type of the system
	({@render c('std::type_index')}), and groups the systems by {@render c('ScheduleLabel')}. Through
	the {@render c('Scene')}:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		{@render c('registerSystem<T>(label)')} creates the system, adds it to the schedule of
		{@render c('label')} ({@render c('Update')} by default) and returns a
		{@render c('std::shared_ptr<T>')} to it.
	</li>
	<li>{@render c('setSystemComponentSet<T>(set)')} sets the components the system requires.</li>
	<li>
		{@render c('removeSystem<T>()')} removes the system from its schedule and forgets its
		requirements.
	</li>
	<li>
		Each time an entity's components change, every system compares the new
		{@render c('ComponentSet')} with its requirements and adds or removes the entity from its set.
	</li>
	<li>When an entity is destroyed, it is removed from every system.</li>
</ul>
{@render code(setupExample)}

<h2 class="mt-9 text-lg font-semibold">Running a system</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Nobody calls a system by hand. Each system is registered with one of the schedule labels:
</p>
{@render code(labelExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('scene.runSchedule(label)')} calls {@render c('update(scene)')} on every system of that
	label, in registration order. It loops over a copy of the list, so a system can remove itself (or
	another one) during its {@render c('update')}. On the client, {@render c('Application::run()')}
	runs the labels in this order, with {@render c('FixedUpdate')} run zero or more times per frame by
	an accumulator, each step followed by {@render c('flushDestroyQueue()')}:
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The server ({@render c('GameLoop::run()')} in {@render c('src/server/game/game_loop.cpp')}) only
	runs {@render c('FixedUpdate')}, once per tick, then flushes the destroy queue. A system loops over
	{@render c('_entities')} and reads its components from the {@render c('Scene')}:
</p>
{@render code(movementExample)}

<h2 class="mt-9 text-lg font-semibold">Plugins</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	On the client, most systems are registered by plugins. A plugin implements
	{@render c('IPlugin')} ({@render c('src/game-rtype/Application.hpp')}):
	{@render c('Application::addPlugin')} calls its {@render c('build')} right away, and
	{@render c('run()')} calls every {@render c('cleanup')} when the loop ends. The
	{@render c('Application')} constructor registers the base components, adds the plugins, then
	registers {@render c('ScriptSystem')} itself:
</p>
{@render code(pluginExample)}

<h2 class="mt-9 text-lg font-semibold">The R-Type systems</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">GetInput</strong> ({@render c('src/plugins/Renderer/systems')},
		{@render c('PrepUpdate')}, no components): reads the bound keys (ZQSD, arrows, space) with
		raylib into the {@render c('KeyboardInput')} resource, and sets {@render c('AppAlive')} to false
		when the window is closed.
	</li>
	<li>
		<strong class="font-semibold">TryConnect</strong>, <strong class="font-semibold">SendInput</strong>,
		<strong class="font-semibold">ReadSocket</strong>
		({@render c('src/plugins/Network/Client/systems')}, no components): connect to the server
		({@render c('PrepUpdate')}), send the pressed keys as input bits and read the server's packets
		({@render c('StateTransition')}).
	</li>
	<li>
		<strong class="font-semibold">MovementSystem</strong> ({@render c('src/plugins/Physics/systems')},
		{@render c('FixedUpdate')}, {@render c('Position')} + {@render c('Velocity')}): moves each
		entity by its velocity multiplied by {@render c('FIXED_DELTA_TIME')}.
	</li>
	<li>
		<strong class="font-semibold">Collider2DSystem</strong>
		({@render c('src/plugins/Physics/systems')}, {@render c('FixedUpdate')},
		{@render c('Position')} + {@render c('Collider2D')}): tests every pair of entities (box or circle
		shapes, two players are skipped) and calls the {@render c('onCollision')} callback of both when
		they overlap.
	</li>
	<li>
		<strong class="font-semibold">ScriptSystem</strong> ({@render c('src/systems')},
		{@render c('FixedUpdate')}, {@render c('Script')}): runs each {@render c('AT_LAUNCH')} script
		once, then every {@render c('ON_UPDATE')} script on each step.
	</li>
	<li>
		<strong class="font-semibold">RenderSystem</strong> ({@render c('src/plugins/Renderer/systems')},
		{@render c('PostUpdate')}, {@render c('Position')} + {@render c('Sprite')}): gets the renderer
		from the {@render c('std::reference_wrapper<IRenderer>')} resource, sorts the entities by
		{@render c('Sprite::drawOrder')} (a stable sort, smallest first) and draws each one at its
		position, so it never calls raylib directly. This sort keeps the stars (draw order -1) behind
		everything else.
	</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	The server registers only {@render c('MovementSystem')}, {@render c('ScriptSystem')} and
	{@render c('Collider2DSystem')}, all in {@render c('FixedUpdate')}, in the
	{@render c('GameLoop')} constructor. The player's velocity is not set by a system: the server's
	{@render c('GameLoop::executeInput')} sets it to {@render c('PLAYER_SPEED')} (400 pixels per second)
	in the direction of the received input bits.
</p>

<h2 class="mt-9 text-lg font-semibold">Adding a system</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	This example (not in the game) destroys the server entities that leave the screen on the left,
	using the {@render c('Position')} component from
	<a href="/developer/ecs-components" class="text-[#ff8a3d] hover:underline">How components work</a>.
</p>
<p class="mt-3 max-w-[100ch] leading-7">1. Create the header and the source in {@render c('src/systems')}:</p>
{@render code(newHeaderExample)}
{@render code(newSourceExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The entities are queued with {@render c('queueDestroy')} instead of being destroyed in the loop,
	because destroying an entity removes it from {@render c('_entities')} while the loop is going
	through it. The queue is flushed after each {@render c('FixedUpdate')} step.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	2. Add the source file to the target that uses it in {@render c('CMakeLists.txt')}
	({@render c('r-type_server')} here, {@render c('${PROJECT_NAME}')} for the client):
</p>
{@render code(newCmakeExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	3. Register the system with its label and set its requirements, in the {@render c('GameLoop')}
	constructor for the server, or in the {@render c('Application')} constructor or a plugin's
	{@render c('build')} for the client. Nothing else is needed: {@render c('runSchedule')} calls it.
</p>
{@render code(newAppExample)}

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Set the requirements right after {@render c('registerSystem')}, before creating entities:
		entities that already have their components are not added afterwards.
	</li>
	<li>
		Registering the same system twice throws a {@render c('RegisterError')}; removing a system that
		is not registered throws a {@render c('ComponentNotRegisteredError')}.
	</li>
	<li>
		{@render c('_entities')} is a {@render c('std::set')}, so entities are always processed in
		increasing id order.
	</li>
	<li>
		Systems of the same label run in registration order: on the client the
		{@render c('FixedUpdate')} order is {@render c('MovementSystem')},
		{@render c('Collider2DSystem')}, {@render c('ScriptSystem')}; on the server it is
		{@render c('MovementSystem')}, {@render c('ScriptSystem')}, {@render c('Collider2DSystem')}.
	</li>
	<li>
		When two opposite keys are pressed, the server adds both directions to the velocity, so they
		cancel out.
	</li>
</ul>
