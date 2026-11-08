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
MovementSystem    Transform + Velocity     no              yes             no
Collider2DSystem  Transform + Collider2D   no              yes             no
ScriptSystem      Script                   no              yes             yes
RenderSystem      Transform + Sprite       yes             -               -
SoundSystem       SoundEffect              yes             -               -`;

	const setupExample = `// Physics2D::build(Application &app)
app._scene.registerSystem<MovementSystem>(FixedUpdate);
ComponentSet movementRequirements;
movementRequirements.set(app._scene.getComponentType<Transform>());
movementRequirements.set(app._scene.getComponentType<Velocity>());
app._scene.setSystemComponentSet<MovementSystem>(movementRequirements);`;

	const labelExample = `enum ScheduleLabel { PrepUpdate, StateTransition, FixedUpdate, Update, PostUpdate, Render, Overlay };`;

	const accessExample = `// Physics2D::build(Application &app)
app._scene.setSystemAccess<GravitySystem>(app._scene.access()
                                              .read<RigidBody2D>()
                                              .write<Velocity>()
                                              .readResource<Gravity2D>());
app._scene.setSystemAccess<MovementSystem>(
    app._scene.access().read<Velocity>().write<Transform>());`;

	const conflictExample = `A               B               run together?
read<T>         read<T>         yes
read<T>         write<T>        no, the one registered first runs first
write<T>        write<T>        no, the one registered first runs first
write<T>        write<U>        yes
(no access)     anything        no, A runs alone on the main thread`;

	const parallelForExample = `void MovementSystem::update(Scene &scene)
{
    const std::vector<Entity> entities(_entities.begin(), _entities.end());

    scene.parallelFor(entities, [&scene](Entity entity) {
        Transform &position = scene.getComponent<Transform>(entity);
        Velocity const &velocity = scene.getComponent<Velocity>(entity);

        position.x += velocity.dx * FIXED_DELTA_TIME;
        position.y += velocity.dy * FIXED_DELTA_TIME;
    });
}

// Or over a range, split in chunks of at least minChunk items (64 by default)
scene.parallelFor(count, [](std::size_t begin, std::size_t end) { ... });`;

	const runExample = `// Application::run()
while (_scene.getResource<AppAlive>().alive && !stopRequested) {   // SIGINT / SIGTERM stop it
    _scene.runSchedule(PrepUpdate);
    _scene.runSchedule(StateTransition);
    for (int ticks = 0; _clock.isTickDue(); ++ticks) {   // at most MAX_CATCH_UP_TICKS (15)
        _scene.runSchedule(FixedUpdate);
        _scene.flushDestroyQueue();
        _clock.advance();
    }
    _scene.runSchedule(Update);
    _scene.runSchedule(PostUpdate);
    if (_renderer)
        renderFrame();                                  // Render, Overlay
    else
        std::this_thread::sleep_until(_clock.getNextTickTime());
}`;

	const movementExample = `void MovementSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Transform &position = scene.getComponent<Transform>(entity);
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
addPlugin(std::make_shared<RayLibRenderer>()); // GetInput, RenderSystem, SoundSystem
addPlugin(std::make_shared<Physics2D>());        // MovementSystem, Collider2DSystem
addPlugin(std::make_shared<Client>(serverAddress, serverPort)); // TryConnect, SendInput, ReadSocket`;

	const newHeaderExample = `// src/systems/OffScreenSystem.hpp
#pragma once
#include "Transform.hpp"
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
        if (scene.getComponent<Transform>(entity).x < -100.0f)
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
offScreenRequirements.set(_scene.getComponentType<Transform>());
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
	{@render c('Transform')} + {@render c('Type')} + {@render c('Sprite')}; the server player also has
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
	label. By default they run one after the other, in registration order; systems that declare their
	accesses can run at the same time (see
	<a href="#parallel-systems" class="text-[#ff8a3d] hover:underline">Running systems in parallel</a>).
	A run works on a snapshot of the schedule, so a system can register or remove a system (itself
	included) during its {@render c('update')}: the change takes effect on the next run. {@render c('Application::run()')} runs the
	labels in this order on the main thread, with {@render c('FixedUpdate')} run once per due 1/60 s
	tick, each followed by {@render c('flushDestroyQueue()')}:
</p>
{@render code(runExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The server runs the same loop without a renderer and sleeps until the next tick. A system loops over
	{@render c('_entities')} and reads its components from the {@render c('Scene')}:
</p>
{@render code(movementExample)}

<h2 id="parallel-systems" class="mt-9 text-lg font-semibold">Running systems in parallel</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A system can declare the components and resources it reads and writes with
	{@render c('scene.setSystemAccess<T>(scene.access()...)')}, right after registering it.
	{@render c('read<C>()')} / {@render c('write<C>()')} declare a component,
	{@render c('readResource<R>()')} / {@render c('writeResource<R>()')} a resource; a write also
	counts as a read:
</p>
{@render code(accessExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	From these declarations, {@render c('runSchedule')} builds a dependency graph for each label
	({@render c('src/ECS/systems/ScheduleGraph.cpp')}), rebuilt only when a system or an access
	changes. Two systems conflict when one writes something the other reads or writes. Systems that
	do not conflict run at the same time on a thread pool; systems that conflict keep their
	registration order:
</p>
{@render code(conflictExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		A system that declares nothing is <strong class="font-semibold">exclusive</strong>: it waits
		for every system registered before it, runs alone on the main thread, and every system after it
		waits for it. Without any declaration a schedule runs exactly as before, sequentially.
	</li>
	<li>
		The thread pool ({@render c('src/ECS/core/ThreadPool.hpp')},
		{@render c('hardware_concurrency() - 1')} workers, at least 1) is only created the first time
		something can run in parallel, so a sequential app starts no thread. The main thread works too
		while it waits.
	</li>
	<li>
		If a system throws, the systems that had not started yet are skipped, the running ones finish,
		then the first exception is rethrown by {@render c('runSchedule')} on the main thread.
	</li>
	<li>
		A declaration is a promise: the system must only touch what it declared. Creating or destroying
		entities, adding or removing components, and registering systems are not thread-safe; a system
		that does one of these must stay exclusive. {@render c('queueDestroy')} is the exception: it
		is thread-safe and can be called from any system.
	</li>
	<li>
		Calls that are not thread-safe themselves (raylib drawing, the window, input) belong in an
		exclusive system.
	</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	Inside one system, {@render c('scene.parallelFor')} ({@render c('src/ECS/core/ParallelFor.hpp')})
	splits a loop over the pool. The work is cut into contiguous chunks of at least
	{@render c('PARALLEL_FOR_MIN_CHUNK')} (64) items, the calling thread runs the first one, and the
	call returns once every chunk is done. Up to {@render c('minChunk')} items, it simply runs the loop
	on the calling thread. The body must be safe to run on disjoint items at the same time; the first
	exception thrown by a chunk is rethrown once every chunk has ended:
</p>
{@render code(parallelForExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The engine plugins declare their accesses for the systems that can safely run in parallel:
	{@render c('GravitySystem')} and {@render c('MovementSystem')} (Physics2D),
	{@render c('Gravity3DSystem')} and {@render c('Movement3DSystem')} (Physics3D),
	{@render c('AnimationSystem')} and {@render c('ParticleEmitter2DSystem')} (2D),
	{@render c('CameraFollowSystem')} and {@render c('ParticleEmitter3DSystem')} (3D),
	{@render c('AudioSystem')}, and {@render c('NetReceiveSystem')}, {@render c('NetSendSystem')},
	{@render c('InputSendSystem')} and {@render c('EventUpdateSystem')} (Network). The colliders, the
	renderers and the systems registered by R-Type itself declare nothing and stay exclusive. The
	gravity, movement and {@render c('Collider2DSystem')} loops use {@render c('parallelFor')}.
</p>

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
		<strong class="font-semibold">MovementSystem</strong> ({@render c('src/plugins/Physics2D/systems')},
		{@render c('FixedUpdate')}, {@render c('Transform')} + {@render c('Velocity')}): moves each
		entity by its velocity multiplied by {@render c('FIXED_DELTA_TIME')}.
	</li>
	<li>
		<strong class="font-semibold">Collider2DSystem</strong>
		({@render c('src/plugins/Physics2D/systems')}, {@render c('FixedUpdate')},
		{@render c('Transform')} + {@render c('Collider2D')}): tests every pair of entities (box or circle
		shapes, filtered by layer and mask). When a trigger touches another collider, it calls the
		{@render c('onCollision')} callbacks every step. When two solid colliders start touching, the
		momentums of their {@render c('RigidBody2D')} are added together and both leave with the same
		{@render c('Velocity')}. While solid colliders touch, they are pushed out of each other and stop
		moving into each other, so bodies rest on a static floor.
	</li>
	<li>
		<strong class="font-semibold">GravitySystem</strong>
		({@render c('src/plugins/Physics2D/systems')}, {@render c('FixedUpdate')}, before the
		{@render c('MovementSystem')}, {@render c('RigidBody2D')} + {@render c('Velocity')}): adds the
		{@render c('Gravity2D')} resource (project settings {@render c('GravityX')} /
		{@render c('GravityY')}, 980 px/s² down by default) times {@render c('gravityScale')} to the
		velocity of every non static body.
	</li>
	<li>
		<strong class="font-semibold">ScriptSystem</strong> ({@render c('src/systems')},
		{@render c('FixedUpdate')}, {@render c('Script')}): runs each {@render c('AT_LAUNCH')} script
		once, then every {@render c('ON_UPDATE')} script on each step.
	</li>
	<li>
		<strong class="font-semibold">RenderSystem</strong> ({@render c('src/plugins/Renderer/systems')},
		{@render c('PostUpdate')}, {@render c('Transform')} + {@render c('Sprite')}): gets the renderer
		from the {@render c('std::reference_wrapper<IRenderer>')} resource, sorts the entities by
		{@render c('Sprite::drawOrder')} (a stable sort, smallest first) and draws each one at its
		position, so it never calls raylib directly. This sort keeps the stars (draw order -1) behind
		everything else.
	</li>
	<li>
		<strong class="font-semibold">AnimationSystem</strong> ({@render c('src/plugins/2D/systems')},
		{@render c('FixedUpdate')}, {@render c('Animator')} + {@render c('Sprite')}): reads each
		{@render c('.anim')} file once (the {@render c('AnimationLibrary')} resource), advances the
		animator's animation at its fps and sets the {@render c('Sprite')}'s texture and region to
		the current frame. {@render c('AnimationSystem::play(animator, "run")')} plays {@render c('run.anim')} from the folder of
		the starting animation; a missing file throws, so a typo is not silent.
	</li>
	<li>
		<strong class="font-semibold">SoundSystem</strong> ({@render c('src/plugins/Renderer/systems')},
		{@render c('PostUpdate')}, {@render c('SoundEffect')}): plays the music of the
		{@render c('BackgroundMusic')} resource when the game adds one (it changes track when
		{@render c('path')} changes and stops when {@render c('enabled')} is false or the resource is
		removed), then plays every {@render c('SoundEffect')} whose {@render c('play')} is true. Like the
		{@render c('RenderSystem')}, it only goes through {@render c('IRenderer')}.
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
	using the {@render c('Transform')} component from
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
		Systems of the same label that conflict (or declare no access) run in registration order: on the client the
		{@render c('FixedUpdate')} order is {@render c('MovementSystem')},
		{@render c('Collider2DSystem')}, {@render c('ScriptSystem')}; on the server it is
		{@render c('MovementSystem')}, {@render c('ScriptSystem')}, {@render c('Collider2DSystem')}.
	</li>
	<li>
		When two opposite keys are pressed, the server adds both directions to the velocity, so they
		cancel out.
	</li>
</ul>
