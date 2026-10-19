<script>
	const treeExample = `src/
├── engine/            # generic engine, no R-Type code
│   ├── core/          # Entity, EntityManager, ComponentPool, ComponentManager, ResourceManager,
│   │                  # Scene, Time, TickTimeManager
│   ├── systems/       # System base class, SystemManager and the ScheduleLabel enum
│   ├── prefabs/       # Prefab and PrefabsManager
│   └── errors/        # EngineError and its subclasses
├── plugins/           # features added to the client Application through IPlugin
│   ├── Renderer/      # IRenderer, RayLibRenderer, Sprite, RenderSystem, GetInput, KeyboardInput
│   ├── Physics/       # Velocity, Collider2D, MovementSystem, Collider2DSystem
│   └── Network/       # Socket and the client systems TryConnect, SendInput, ReadSocket
├── components/        # shared components: Position, PlayerId, Script, Shape, Type
├── systems/           # shared systems: ScriptSystem
├── ressources/        # shared resources: AppAlive
├── common/            # keyboard codes and the network protocol
├── game-rtype/        # client Application (setup and game loop) and Game (waves, stars, movement)
└── server/            # server GameLoop, network thread and input queues`;

	const sceneExample = `class Scene {
  public:
    Entity createEntity();
    bool isEntityAlive(Entity entity) const;
    void destroyEntity(Entity entity);
    void queueDestroy(Entity entity);
    void flushDestroyQueue();
    void putAllEntitysInDestroyQueue();
    void runSchedule(ScheduleLabel label);

    template <typename Component> void registerComponent();
    template <typename Component> void addComponent(Entity entity, Component component);
    template <typename Component> void removeComponent(Entity entity);
    template <typename Component> Component &getComponent(Entity entity);
    template <typename Component> Component &getComponent(Entity entity, std::size_t componentIndex);
    template <typename Component> std::size_t getComponentCount(Entity entity);
    template <typename Component> ComponentType getComponentType();

    template <typename SystemType> std::shared_ptr<SystemType> registerSystem(ScheduleLabel label = Update);
    template <typename SystemType> void setSystemComponentSet(ComponentSet componentSet);
    template <typename SystemType> void removeSystem();

    template <typename Resource, typename... Args> Resource &addResource(Args &&...args);
    template <typename Resource> Resource &getResource();
    template <typename Resource> bool hasResource() const;
    template <typename Resource> void removeResource();

  private:
    std::unique_ptr<EntityManager> _entityManager;
    std::unique_ptr<ComponentManager> _componentManager;
    std::unique_ptr<SystemManager> _systemManager;
    std::unique_ptr<ResourceManager> _resourceManager;
    std::queue<Entity> _destroyQueue;
};`;

	const exampleCode = `Scene scene;

// 1. Register the component types
scene.registerComponent<Position>();
scene.registerComponent<Velocity>();

// 2. Register a system in a schedule and tell it which components it needs
std::shared_ptr<MovementSystem> movement = scene.registerSystem<MovementSystem>(FixedUpdate);
ComponentSet required;
required.set(scene.getComponentType<Position>());
required.set(scene.getComponentType<Velocity>());
scene.setSystemComponentSet<MovementSystem>(required);

// 3. Create an entity and give it components
Entity ship = scene.createEntity();
scene.addComponent(ship, Position{0.0f, 200.0f});
scene.addComponent(ship, Velocity{100.0f, 0.0f}); // ship is now in movement->_entities

// 4. Run every system of the schedule, then destroy the queued entities
scene.runSchedule(FixedUpdate);
scene.flushDestroyQueue();`;

	const scheduleExample = `enum ScheduleLabel { PrepUpdate, StateTransition, FixedUpdate, Update, PostUpdate };`;

	const resourceExample = `scene.addResource<AppAlive>();                     // one AppAlive for the whole scene
bool alive = scene.getResource<AppAlive>().alive;  // ResourceNotFoundError if never added`;

	const prefabExample = `PrefabsManager prefabs;
prefabs.createPrefab("Enemy", Prefab()
                                  .addComponent(Position{0.0f, 0.0f})
                                  .addComponent(Velocity{-100.0f, 0.0f}));

Entity enemy = prefabs.InstantiatePrefab("Enemy", scene); // new entity with a copy of each component`;
</script>

<svelte:head>
	<title>How the ECS works - R-Type docs</title>
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

<h1 class="text-3xl font-semibold">How the ECS works</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">
	The architecture of the game engine and how its parts fit together
</p>

<p class="max-w-[100ch] leading-7">
	The game engine is built on an <strong class="font-semibold">ECS</strong> (Entity Component System).
	Instead of a class hierarchy such as {@render c('Player')} inheriting from {@render c('Ship')}
	inheriting from {@render c('GameObject')}, a game object is only an id, its data is split into small
	components, and the logic lives in systems that process every object having the right components. A
	behaviour is added to an object by giving it a component, without writing a new class.
</p>

<h2 class="mt-9 text-lg font-semibold">The three parts</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		<strong class="font-semibold">Entity</strong>: a number that identifies a game object (a ship, a
		bullet, a wall). It holds no data by itself. See
		<a href="/developer/ecs-entities" class="text-[#ff8a3d] hover:underline">How entities work</a>.
	</li>
	<li>
		<strong class="font-semibold">Component</strong>: a plain struct of data attached to an entity,
		for example a {@render c('Position')} or a {@render c('Velocity')}. It has no logic. See
		<a href="/developer/ecs-components" class="text-[#ff8a3d] hover:underline"
			>How components work</a
		>.
	</li>
	<li>
		<strong class="font-semibold">System</strong>: the logic. A system declares which components it
		needs and runs on every entity that has all of them, for example moving every entity that has a
		{@render c('Position')} and a {@render c('Velocity')}. See
		<a href="/developer/ecs-systems" class="text-[#ff8a3d] hover:underline">How systems work</a>.
	</li>
</ul>

<h2 class="mt-9 text-lg font-semibold">Files</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('src/engine')} is the generic engine and does not know anything about R-Type or raylib.
	The R-Type specific code (its components, its systems, the plugins, the client application and the
	server) lives next to it.
</p>
{@render code(treeExample)}

<h2 class="mt-9 text-lg font-semibold">The Scene</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('Scene')} ({@render c('src/engine/core/Scene.hpp')}) is the only class the game talks
	to. It owns the managers and forwards each call to the right one, so the game never uses them
	directly:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('EntityManager')} gives out entity ids and stores which components each entity has.</li>
	<li>{@render c('ComponentManager')} stores the component values, one pool per component type.</li>
	<li>
		{@render c('SystemManager')} stores the systems, the schedule each one runs in and the list of
		entities each one runs on.
	</li>
	<li>{@render c('ResourceManager')} stores the resources (see below).</li>
</ul>
{@render code(sceneExample)}

<h2 class="mt-9 text-lg font-semibold">A complete example</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	This is the order used by the game: register the components, register the systems, then create the
	entities. On the client, the {@render c('Application')} constructor registers the base components
	and systems, and each plugin registers its own in its {@render c('build')} function (for example
	{@render c('Physics::build')} sets up {@render c('MovementSystem')}). It creates no entity: they are
	created by {@render c('ReadSocket')} from the server snapshots. On the server, {@render c('GameLoop')}
	({@render c('src/server/game/game_loop.cpp')}) does both in its constructor and while the game runs.
</p>
{@render code(exampleCode)}

<h2 class="mt-9 text-lg font-semibold">Schedules</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each system is registered in one schedule ({@render c('Update')} if none is given), and the game
	never calls {@render c('update')} directly: {@render c('runSchedule(label)')} updates every system
	of that schedule in registration order.
</p>
{@render code(scheduleExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Each client frame runs {@render c('PrepUpdate')}, {@render c('StateTransition')}, then
	{@render c('FixedUpdate')} zero or more times at a fixed rate, then {@render c('Update')} and
	{@render c('PostUpdate')}. The server only runs {@render c('FixedUpdate')}, once per tick.
</p>

<h2 class="mt-9 text-lg font-semibold">Resources</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A resource is a value that exists once per scene and belongs to no entity, such as the renderer,
	the keyboard input, the network {@render c('Socket')} or {@render c('AppAlive')}. There is at most
	one resource of each type; adding it again replaces it.
</p>
{@render code(resourceExample)}

<h2 class="mt-9 text-lg font-semibold">Prefabs</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A {@render c('Prefab')} is a list of components to add to a new entity. The
	{@render c('PrefabsManager')} stores prefabs by name and creates an entity from one in a single
	call. The server defines four prefabs in {@render c('GameLoop::initializePrebabs')}:
	{@render c('Enemy')}, {@render c('Enemy2')}, {@render c('PlayerBullet')} and {@render c('Star')}.
	It creates 20 stars at startup, then the two spawner scripts create enemies and stars. Asking for a
	name that was never created gives an empty entity, without any error.
</p>
{@render code(prefabExample)}

<h2 class="mt-9 text-lg font-semibold">What the Scene does for you</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	When you call {@render c('addComponent')} or {@render c('removeComponent')}, the Scene keeps the
	three managers in sync:
</p>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-5 leading-7">
	<li>The component value is added to (or removed from) its pool.</li>
	<li>The bit of this component type is set (or cleared) in the entity's {@render c('ComponentSet')}.</li>
	<li>
		Every system checks the new {@render c('ComponentSet')}: the entity joins the systems whose
		requirements it now meets, and leaves the others.
	</li>
</ol>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('destroyEntity')} does the same in one call: the id is freed, the entity's components are
	removed from every pool, and the entity is removed from every system. Destroying an entity that is
	not alive does nothing.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	A system should not destroy an entity while systems are running. It calls
	{@render c('queueDestroy')} instead, and the game calls {@render c('flushDestroyQueue')} after each
	{@render c('FixedUpdate')} to destroy every queued entity.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		There can be at most {@render c('MAX_ENTITIES')} (4096) entities alive and
		{@render c('MAX_COMPONENTS')} (32) component types ({@render c('src/engine/core/Entity.hpp')}).
		Creating one more entity throws {@render c('EntityLimitError')}.
	</li>
	<li>
		Register a component type before using it in any other call, otherwise
		{@render c('ComponentNotRegisteredError')} is thrown. Registering it twice throws
		{@render c('MultipleComponentDefinition')}.
	</li>
	<li>
		Adding a component an entity already has throws {@render c('MultipleComponentDefinition')},
		unless {@render c('AllowMultiple<T>')} is specialized to true for that type (as
		{@render c('Script')} does). Getting or removing
		a component the entity does not have throws {@render c('MissingComponentError')}.
	</li>
	<li>
		Registering the same system twice throws {@render c('RegisterError')}. Calling
		{@render c('setSystemComponentSet')} or {@render c('removeSystem')} on a system that is not
		registered throws {@render c('ComponentNotRegisteredError')}.
	</li>
	<li>
		Set a system's required components right after registering it, before creating entities. Systems
		are only updated when a component is added or removed, so entities that already have their
		components when the requirements are set are not added to the system.
	</li>
	<li>
		The game loop that calls the systems is described in
		<a href="/developer/game-loop" class="text-[#ff8a3d] hover:underline">How the game loop works</a>.
	</li>
</ul>
