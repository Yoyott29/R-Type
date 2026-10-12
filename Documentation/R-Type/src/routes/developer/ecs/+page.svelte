<script>
	const treeExample = `src/
├── engine/            # generic engine, no R-Type code
│   ├── core/          # Entity, EntityManager, ComponentPool, ComponentManager, Scene, Time
│   ├── systems/       # System base class and SystemManager
│   ├── renderer/      # IRenderer interface and ColorRGBA
│   └── errors/        # EngineError and its subclasses
├── components/        # R-Type components: Position, Velocity, Sprite, PlayerControlled
├── systems/           # R-Type systems: InputSystem, MovementSystem, RenderSystem
└── game-rtype/        # Application (setup and game loop) and RaylibRenderer`;

	const sceneExample = `class Scene {
  public:
    Entity createEntity();
    void destroyEntity(Entity entity);

    template <typename Component> void registerComponent();
    template <typename Component> void addComponent(Entity entity, Component component);
    template <typename Component> void removeComponent(Entity entity);
    template <typename Component> Component &getComponent(Entity entity);
    template <typename Component> ComponentType getComponentType();

    template <typename SystemType> std::shared_ptr<SystemType> registerSystem();
    template <typename SystemType> void setSystemComponentSet(ComponentSet componentSet);

  private:
    std::unique_ptr<EntityManager> _entityManager;
    std::unique_ptr<ComponentManager> _componentManager;
    std::unique_ptr<SystemManager> _systemManager;
};`;

	const exampleCode = `Scene scene;

// 1. Register the component types
scene.registerComponent<Position>();
scene.registerComponent<Velocity>();

// 2. Register a system and tell it which components it needs
std::shared_ptr<MovementSystem> movement = scene.registerSystem<MovementSystem>();
ComponentSet required;
required.set(scene.getComponentType<Position>());
required.set(scene.getComponentType<Velocity>());
scene.setSystemComponentSet<MovementSystem>(required);

// 3. Create an entity and give it components
Entity ship = scene.createEntity();
scene.addComponent(ship, Position{0.0f, 200.0f});
scene.addComponent(ship, Velocity{100.0f, 0.0f}); // ship is now in movement->_entities

// 4. Run the system
movement->update(scene);`;
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
	The R-Type specific code (its components, its systems and the application) lives next to it.
</p>
{@render code(treeExample)}

<h2 class="mt-9 text-lg font-semibold">The Scene</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('Scene')} ({@render c('src/engine/core/Scene.hpp')}) is the only class the game talks
	to. It owns the three managers and forwards each call to the right one, so the game never uses them
	directly:
</p>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('EntityManager')} gives out entity ids and stores which components each entity has.</li>
	<li>{@render c('ComponentManager')} stores the component values, one pool per component type.</li>
	<li>{@render c('SystemManager')} stores the systems and the list of entities each one runs on.</li>
</ul>
{@render code(sceneExample)}

<h2 class="mt-9 text-lg font-semibold">A complete example</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	This is the order used by the game in {@render c('Application')}: register the components, register
	the systems, then create the entities.
</p>
{@render code(exampleCode)}

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

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		Register a component type before using it in any other call, otherwise
		{@render c('ComponentNotRegisteredError')} is thrown.
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
