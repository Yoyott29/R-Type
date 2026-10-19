<script>
	const enemyExample = `// src/server/game/game_loop.cpp, GameLoop::initializePrebabs()
_prefabs.createPrefab(
    "Turret", Prefab()
                  .addComponent(Type{EntType::ENEMY})
                  .addComponent(Position{2000.0f, 0.0f})
                  .addComponent(Velocity{-150.0f, 0.0f})
                  .addComponent(Sprite{BoxShape{50.0f, 50.0f}, ColorRGBA{200, 200, 0, 255}})
                  .addComponent(Collider2D{BoxShape{50.0f, 50.0f}, nullptr})
                  .addComponent(Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &enemy) {
                                           _game.DestroyOffScreenEntity(scene, enemy);
                                       }}));`;

	const spawnExample = `// anywhere the game loop runs, for example in a spawner Script
Entity turret = _prefabs.InstantiatePrefab("Turret", _scene);
_scene.getComponent<Position>(turret).y = 540.0f;
_entities.push_back(turret);   // only entities in _entities are sent in the snapshots`;

	const textureExample = `// 1. src/common/rtype_protocol.hpp: a new variant value
constexpr uint8_t ENEMY_VARIANT_3 = 2;

// 2. src/server/game/game_loop.cpp, GameLoop::buildSnapshot(): send it
if (sprite.texturePath == "Assets/Turret.png")
    variant = rtype_protocol::ENEMY_VARIANT_3;

// 3. src/plugins/Network/Client/systems/ReadSocket.cpp, texturePathForType(): pick it
case EntType::ENEMY:
    if (variant == rtype_protocol::ENEMY_VARIANT_3)
        return "Assets/Turret.png";
    ...`;

	const packetExample = `// 1. src/common/rtype_protocol.hpp
enum MessageType : uint8_t {
    ...
    PAUSE = 0x21,            // [PAUSE]   client -> server, category 2 (game)
};
constexpr uint8_t PROTOCOL_VERSION = 4;   // the format changed

// 2. server: src/server/network/network.cpp, Network::receiveLoop()
case rtype_protocol::PAUSE:
    if (n != 1 || !playerId)
        break;               // check the size before reading anything
    ...                      // hand it to the game thread, never touch the Scene here
    break;

// 3. client: ReadSocket::handlePacket() for a server -> client packet,
//    or a system like SendInput for a client -> server packet`;

	const pluginExample = `// src/plugins/Audio/Audio.hpp
#pragma once
#include "Application.hpp"

class Audio : public IPlugin
{
  public:
    void build(Application &app) override;    // register components, resources, systems
    void cleanup(Application &app) override;  // free what build() opened
};

// src/plugins/Audio/Audio.cpp
void Audio::build(Application &app)
{
    app._scene.registerComponent<SoundEmitter>();
    app._scene.registerSystem<PlaySounds>(PostUpdate);
    ComponentSet requirements;
    requirements.set(app._scene.getComponentType<SoundEmitter>());
    app._scene.setSystemComponentSet<PlaySounds>(requirements);
}

// src/game-rtype/Application.cpp, in the constructor
addPlugin(std::make_shared<Audio>());`;
</script>

<svelte:head>
	<title>How-to guides - R-Type docs</title>
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

{#snippet a(href, text)}<a {href} class="text-[#ff8a3d] hover:underline">{text}</a>{/snippet}

<h1 class="text-3xl font-semibold">How-to guides</h1>
<p class="mt-1 mb-8 text-[15px] text-[#7f9189]">Step by step recipes for the most common changes</p>

<p class="max-w-[100ch] leading-7">
	Each guide lists the files to change, in order. The names used in the examples
	({@render c('Turret')}, {@render c('PAUSE')}, {@render c('Audio')}...) do not exist in the code:
	replace them with yours. After any change, build and run {@render c('./tests/tests.sh')}.
</p>

<h2 class="mt-9 text-lg font-semibold">Add a component or a system</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	These two have their own guides, with code that compiles against the current engine:
	{@render a('/developer/ecs-components', 'How components work')} (section "Adding a component") and
	{@render a('/developer/ecs-systems', 'How systems work')} (section "Adding a system"). Remember to add
	every new {@render c('.cpp')} file to {@render c('CMakeLists.txt')}, in the client target, the
	server target, or both.
</p>

<h2 class="mt-9 text-lg font-semibold">Add a new kind of enemy</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-5 leading-7">
	<li>
		Describe it as a prefab in {@render c('GameLoop::initializePrebabs')}, with at least a
		{@render c('Type')}, a {@render c('Position')}, a {@render c('Sprite')} (its size and color are
		sent to the clients) and a script that destroys it off screen.
	</li>
	<li>
		Give it a {@render c('Collider2D')} so the existing callbacks see it: player bullets destroy an
		{@render c('ENEMY')}, and an {@render c('ENEMY')} eliminates the player it touches.
	</li>
	<li>
		Create it with {@render c('InstantiatePrefab')} and push it into {@render c('_entities')}, or
		the snapshots will not include it.
	</li>
	<li>
		Behaviour goes in {@render c('Script')} components or in {@render c('Game')}, like
		{@render c('Game::makeSinusoidalMovement')}. An entity can have several scripts.
	</li>
</ol>
{@render code(enemyExample)}
{@render code(spawnExample)}

<h2 class="mt-9 text-lg font-semibold">Add a texture</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	The texture path is not sent over the network: the client picks it from the type, the variant and
	the player id. To give a new texture to an entity:
</p>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-5 leading-7">
	<li>Put the PNG in {@render c('Assets/')}. Names are case-sensitive on Linux.</li>
	<li>Set the same path in the {@render c('Sprite')} of the server prefab.</li>
	<li>Add a variant value if the type already has a texture, and send it from {@render c('buildSnapshot')}.</li>
	<li>Return the path in {@render c('texturePathForType')} on the client.</li>
	<li>Raise {@render c('PROTOCOL_VERSION')} if the meaning of a field changed.</li>
</ol>
{@render code(textureExample)}

<h2 class="mt-9 text-lg font-semibold">Add a packet type</h2>
<ol class="mt-3 max-w-[100ch] list-decimal space-y-1.5 pl-5 leading-7">
	<li>
		Pick a free value in {@render c('MessageType')}: bit 7 is the direction (set for server to
		client), bits 6 to 4 the category.
	</li>
	<li>Add its size as a constant, and raise {@render c('PROTOCOL_VERSION')}.</li>
	<li>
		Handle it on the receiving side. Always check the size first, and on the server, only queue the
		message for the game thread.
	</li>
	<li>
		Document it in {@render a('/developer/network-protocol', 'How the protocol works')}, update
		{@render c('VERSION')} in {@render c('tests/tests.sh')} and add a test.
	</li>
</ol>
{@render code(packetExample)}

<h2 class="mt-9 text-lg font-semibold">Add a client plugin</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	A plugin groups the components, resources and systems of one subsystem, like
	{@render c('Physics')} or {@render c('RayLibRenderer')}. Put it in
	{@render c('src/plugins/<Name>')}, add its folders to the include directories and its sources to
	the client target in {@render c('CMakeLists.txt')}, then add it in the {@render c('Application')}
	constructor:
</p>
{@render code(pluginExample)}
<p class="mt-3 max-w-[100ch] leading-7">
	Plugins are built in the order they are added. A plugin that uses a component of another plugin
	must be added after it.
</p>

<h2 class="mt-9 text-lg font-semibold">Add a test</h2>
<p class="mt-3 max-w-[100ch] leading-7">
	See the section "Adding a test" of {@render a('/developer/tests-sh', 'How tests.sh works')}.
</p>
