#include "ActionInput.hpp"
#include "Check.hpp"
#include "EngineError.hpp"
#include "InputBindings.hpp"
#include "KeyboardInput.hpp"
#include "LocalPeer.hpp"
#include "Loopback.hpp"
#include "NetComponents.hpp"
#include "Networked.hpp"

namespace
{
const std::string PLAYER = "Player";
constexpr std::size_t FIRE = 2;
} // namespace

TEST_CASE(bindingsResolveSeveralKeys)
{
    InputBindings bindings;
    bindings.bind(Key::Z, FIRE);
    bindings.bind(Key::Up, FIRE);
    KeyboardInput keyboard;
    keyboard.pressed.set(static_cast<std::size_t>(Key::Up));
    ActionInput const actions = bindings.resolve(keyboard);
    CHECK(actions.isActive(FIRE) && actions.active.count() == 1);
    CHECK_THROWS(bindings.bind(Key::S, MAX_ACTIONS), EngineError);
}

TEST_CASE(actionInputReachesRemoteInput)
{
    Setup const setup = [](Scene &scene) { addNetworkedPrefab(scene, PLAYER); };
    NetworkConfig config = NetworkConfig::server(0, "127.0.0.1");
    config.playerPrefabs = {PLAYER};
    NetApp server = makeNetApp(config, setup);
    NetApp client = connectedClient(server, setup);
    PeerId const id = client.scene().getResource<LocalPeer>().id;
    client.scene().addResource<ActionInput>().active.set(FIRE);
    Scene &scene = server.scene();
    CHECK(pollUntil({&server, &client}, [&] {
        for (Entity entity : scene.view<RemoteInput>())
            if (scene.getComponent<Networked>(entity).owner == id)
                return scene.getComponent<RemoteInput>(entity).input.isActive(FIRE);
        return false;
    }));
}
