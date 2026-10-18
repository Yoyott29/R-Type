<script>
	const componentsExample = `// src/components
struct Position {
    float x;
    float y;
};

struct PlayerId {
    std::uint8_t value;
};

enum EntType { PLAYER, ENEMY, BULLET, OBSTACLE };
struct Type {
    EntType type;
};

enum class Timing { AT_LAUNCH, ON_UPDATE };
struct Script {
    Timing timing;
    std::function<void(Scene &, Entity &)> runScript;
    bool isRunning = false;
};

struct BoxShape {    // Shape.hpp: not components, used inside Sprite and Collider2D
    float width;
    float height;
};
struct CircleShape {
    float radius;
};`;

	const pluginComponentsExample = `// src/plugins/Physics/components
struct Velocity {
    float dx; // pixels per second
    float dy;
};

struct Collider2D {
    std::variant<BoxShape, CircleShape> shape;
    std::function<void(Entity, Entity)> onCollision;
};

// src/plugins/Renderer/components
struct Sprite {
    std::variant<BoxShape, CircleShape> shape;
    ColorRGBA color;
};`;

	const registerExample = `// client: Application constructor, then each plugin's build()
_scene.registerComponent<Position>();      // ComponentType 0
_scene.registerComponent<Script>();        // ComponentType 1
_scene.registerComponent<Type>();          // ComponentType 2
app._scene.registerComponent<Sprite>();    // ComponentType 3 (RayLibRenderer::build)
app._scene.registerComponent<Velocity>();  // ComponentType 4 (Physics::build)
app._scene.registerComponent<Collider2D>(); // ComponentType 5 (Physics::build)`;

	const useExample = `scene.addComponent(player, Position{0.0f, 200.0f});

Position &position = scene.getComponent<Position>(player);
position.x += 10.0f; // modifies the value stored in the pool

scene.removeComponent<Position>(player);`;

	const multipleExample = `// src/components/Script.hpp
template <> struct AllowMultiple<Script> : std::true_type {};

// src/systems/ScriptSystem.cpp
for (std::size_t i = 0; i < scene.getComponentCount<Script>(entity); i++) {
    Script &script = scene.getComponent<Script>(entity, i);
    ...
}`;

	const poolExample = `std::array<std::vector<std::size_t>, MAX_ENTITIES> _entityToPackedIndices; // entity -> indices, empty if none
std::vector<Component> _packedComponents;                                    // the values, with no holes
std::vector<Entity> _packedIndexToEntity;                                    // index -> entity`;

	const insertExample = `entities 3, 7 and 12 have a Position

_packedComponents       [ P3 | P7 | P12 ]
_packedIndexToEntity    [  3 |  7 |  12 ]
_entityToPackedIndices  3 -> {0}, 7 -> {1}, 12 -> {2}, every other entity -> {}`;

	const removeExample = `removeComponent(3): the last value (P12) is moved into the hole

_packedComponents       [ P12 | P7 ]
_packedIndexToEntity    [  12 |  7 ]
_entityToPackedIndices  12 -> {0}, 7 -> {1}, 3 -> {}`;

	const newComponentExample = `// src/components/Health.hpp
#pragma once

struct Health {
    int current;
    int max;
};`;

	const newRegisterExample = `// src/game-rtype/Application.cpp (client)
Application::Application(const std::string &serverAddress, uint16_t serverPort)
{
    _scene.registerComponent<Position>();
    ...
    _scene.registerComponent<Health>();
    ...
}

// src/server/game/game_loop.cpp (server)
GameLoop::GameLoop(Network &network, std::atomic<bool> &running)
    : _network(network), _running(running), _game(_prefabs)
{
    _scene.registerComponent<Position>();
    ...
    _scene.registerComponent<Health>();
    ...
}

void GameLoop::initializePlayer(uint8_t id)
{
    Entity playerEntity = _scene.createEntity();
    ...
    _scene.addComponent(playerEntity, Health{3, 3});
}`;

	const prefabExample = `// src/server/game/game_loop.cpp
_prefabs.createPrefab("Enemy", Prefab()
                                   .addComponent(Type{EntType::ENEMY})
                                   .addComponent(Position{2000.0f, 0.0f})
                                   .addComponent(Health{1, 1}));

Entity enemy = _prefabs.InstantiatePrefab("Enemy", _scene); // createEntity() + every addComponent`;
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
<p class="mt-3 max-w-[100ch] leading-7">
	The components shared by the whole game are in {@render c('src/components')}:
</p>
{@render code(componentsExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The components that belong to a plugin are in that plugin's {@render c('components')} folder:
</p>
{@render code(pluginComponentsExample)}
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>{@render c('Position')}: the position of the entity on screen, in pixels.</li>
	<li>{@render c('PlayerId')}: the id of the player that owns the entity (server only).</li>
	<li>
		{@render c('Type')}: what the entity is ({@render c('PLAYER')}, {@render c('ENEMY')},
		{@render c('BULLET')} or {@render c('OBSTACLE')}).
	</li>
	<li>
		{@render c('Script')}: a function run by the {@render c('ScriptSystem')}, at launch or on every
		update. An entity can have several of them, see below.
	</li>
	<li>{@render c('Velocity')}: the speed of the entity, in pixels per second.</li>
	<li>
		{@render c('Collider2D')}: the hit box of the entity (a {@render c('BoxShape')} or a
		{@render c('CircleShape')}) and the function called when it touches another entity.
	</li>
	<li>
		{@render c('Sprite')}: how the entity is drawn. For now it is a coloured
		{@render c('BoxShape')} or {@render c('CircleShape')}.
	</li>
</ul>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('src/plugins/Network/components/Socket.hpp')} is stored as a resource
	({@render c('addResource<Socket>')}), not as a component.
</p>

<h2 class="mt-9 text-lg font-semibold">Registering a component</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each component type must be registered once, before it is used. Registering gives the type its
	{@render c('ComponentType')}, which is its bit in the entities' {@render c('ComponentSet')}, and
	creates the pool that stores its values. Registration is split between the
	{@render c('Application')} constructor, which registers the basic components, and the
	{@render c('build')} function of each plugin, which registers its own. Ids are given in
	registration order, so on the client they are:
</p>
{@render code(registerExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	The server has its own {@render c('Scene')} and registers its own set in the
	{@render c('GameLoop')} constructor ({@render c('src/server/game/game_loop.cpp')}), so the same
	component can have a different id on the client and on the server.
</p>
<p class="mt-3 max-w-[100ch] leading-7">
	Registering the same type twice throws {@render c('MultipleComponentDefinition')}. Using a type
	that is not registered throws {@render c('ComponentNotRegisteredError')}.
</p>

<h2 class="mt-9 text-lg font-semibold">Using components</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('getComponent')} returns a reference to the value stored in the pool, so modifying it
	modifies the entity. Asking for a component the entity does not have throws
	{@render c('MissingComponentError')}.
</p>
{@render code(useExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('Scene::addComponent')} and {@render c('Scene::removeComponent')} also update the
	entity's {@render c('ComponentSet')} and tell the {@render c('SystemManager')}, so the entity is
	added to or removed from the systems that need this component. Removing a component the entity
	does not have throws {@render c('MissingComponentError')}, and using an entity id greater than or
	equal to {@render c('MAX_ENTITIES')} in {@render c('addComponent')} throws
	{@render c('OutOfRangeEntityError')}.
</p>

<h3 class="mt-6 font-semibold">Several components of the same type</h3>
<p class="mt-3 max-w-[100ch] leading-7">
	By default an entity has at most one value of each type: adding a component it already has throws
	{@render c('MultipleComponentDefinition')}. A type can allow several values per entity by
	specializing {@render c('AllowMultiple')} (from {@render c('ComponentPool.hpp')}), as
	{@render c('Script')} does. The values are then read by index:
</p>
{@render code(multipleExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	{@render c('getComponent(entity)')} returns the first value (index 0), and
	{@render c('removeComponent')} removes every value of that type from the entity.
</p>

<h2 class="mt-9 text-lg font-semibold">How values are stored: the ComponentPool</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	Each component type has its own {@render c('ComponentPool')}
	({@render c('src/engine/core/ComponentPool.hpp')}). It is a <strong class="font-semibold"
		>sparse set</strong
	>: the values are packed in one contiguous array, without holes, and two index arrays link entities
	and positions in that array. Each entity has a list of indices, so that it can hold several values
	of an {@render c('AllowMultiple')} type; an empty list means the entity has no such component.
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
	hole, the last value of the array is moved into its place and the index of its entity is updated:
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
	1. Create a header with a plain struct, in {@render c('src/components')} if it is shared, or in
	{@render c('src/plugins/<Plugin>/components')} if only one plugin uses it:
</p>
{@render code(newComponentExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	2. Register it in the {@render c('Application')} constructor (or in the {@render c('build')}
	function of its plugin) and, if the server uses it, in the {@render c('GameLoop')} constructor.
	Then give it to the entities that need it. The entities of the game are created by the server:
</p>
{@render code(newRegisterExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Entities can also be built from a {@render c('Prefab')} ({@render c('src/engine/prefabs/Prefab.hpp')}),
	a list of components added to a new entity on each instantiation:
</p>
{@render code(prefabExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	A component is a header only, so there is nothing to add to {@render c('CMakeLists.txt')}:
	{@render c('src/components')} and the plugin {@render c('components')} folders are already in the
	include path of both executables (except {@render c('src/plugins/Network/components')}, which the
	server does not include). A new plugin folder must be added to {@render c('target_include_directories')}.
	Then write or update a system to use
	it, see <a href="/developer/ecs-systems" class="text-[#ff8a3d] hover:underline">How systems work</a>.
</p>

<h2 class="mt-9 text-lg font-semibold">Good to know</h2>
<ul class="mt-3 max-w-[100ch] list-disc space-y-1.5 pl-5 leading-7">
	<li>
		The engine supports up to {@render c('MAX_COMPONENTS')} (32) component types. This is not
		checked at registration: a 33rd type only fails when its bit is set in a
		{@render c('ComponentSet')}.
	</li>
	<li>
		Adding a component an entity already has throws {@render c('MultipleComponentDefinition')},
		unless the type allows multiple values. Use {@render c('getComponent')} to change it.
	</li>
	<li>
		{@render c('addComponent')} copies the value into the pool, so keep components small and simple.
	</li>
</ul>
