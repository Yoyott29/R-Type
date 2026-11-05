#include "Check.hpp"
#include "EngineError.hpp"
#include "Events.hpp"
#include "LocalPeer.hpp"
#include "Loopback.hpp"
#include "NetEvents.hpp"
#include "Networked.hpp"
#include <optional>

namespace
{
const std::string PLAYER = "Player";

PeerId localId(NetApp &client) { return client.scene().getResource<LocalPeer>().id; }

// The prefab of the only entity owned by peer, nullopt if there is not exactly one
std::optional<std::string> ownedPrefab(Scene &scene, PeerId peer)
{
    std::optional<std::string> prefab;
    int owned = 0;
    for (Entity entity : scene.view<Networked>())
        if (scene.getComponent<Networked>(entity).owner == peer && ++owned == 1)
            prefab = scene.getComponent<Networked>(entity).prefab;
    return owned == 1 ? prefab : std::nullopt;
}
} // namespace

TEST_CASE(unknownPlayerPrefabThrows)
{
    NetworkConfig config = NetworkConfig::server(0, "127.0.0.1");
    config.playerPrefabs = {"Ghost"};
    NetApp server = makeNetApp(config);
    CHECK_THROWS(step(server), EngineError);
}

TEST_CASE(playerSpawnedAndMirroredWithOwner)
{
    Setup const setup = [](Scene &scene) { addNetworkedPrefab(scene, PLAYER); };
    NetworkConfig config = NetworkConfig::server(0, "127.0.0.1");
    config.playerPrefabs = {PLAYER};
    NetApp server = makeNetApp(config, setup);
    NetApp client = connectedClient(server, setup);
    PeerId const id = localId(client);
    CHECK(pollUntil({&server, &client}, [&] { return ownedPrefab(client.scene(), id) == PLAYER; }));
    CHECK(ownedPrefab(server.scene(), id) == PLAYER);
}

TEST_CASE(peerLeftBroadcastToOthers)
{
    NetApp server = makeServer();
    NetApp leaving = connectedClient(server);
    NetApp other = connectedClient(server, {}, {&leaving});
    PeerId const id = localId(leaving);
    leaving.plugin->cleanup(*leaving.app);

    EventReader<PeerLeft> reader;
    std::optional<PeerLeft> left;
    CHECK(pollUntil({&server, &other}, [&] {
        reader.read(other.scene(), [&](PeerLeft const &event) { left = event; });
        return left.has_value();
    }));
    CHECK(left->peer == id && left->reason == DisconnectReason::Remote);
}
