#include "Check.hpp"
#include "GameClock.hpp"
#include "Loopback.hpp"
#include "Messages.hpp"
#include "MirrorSystem.hpp"
#include "NetComponents.hpp"
#include "Networked.hpp"
#include "Position.hpp"
#include "PrefabsManager.hpp"
#include "ReservedInbox.hpp"

namespace
{
constexpr std::uint32_t NET_ID = 1;
const std::string BOX = "Box";

void addBoxPrefab(Scene &scene) { addNetworkedPrefab(scene, BOX); }

NetMessage entityState(std::uint32_t tick)
{
    EntityStateMessage const message{tick, NET_ID, NO_PEER, BOX, {}};
    return NetMessage{0, static_cast<std::uint16_t>(ReservedType::EntityState), message.encode()};
}

NetMessage despawn(std::uint32_t tick)
{
    std::vector<std::uint32_t> const ids{NET_ID};
    return NetMessage{0, static_cast<std::uint16_t>(ReservedType::Despawn),
                      DespawnMessage::encodeSplit(tick, ids).at(0)};
}

// A client with no server, fed by hand to its own MirrorSystem
struct InjectedClient {
    NetApp app = makeNetApp(NetworkConfig::client("127.0.0.1", 1), addBoxPrefab);
    MirrorSystem mirrors;

    Scene &scene() { return app.scene(); }

    // How many mirrors are left once messages are read
    std::size_t inject(std::vector<NetMessage> messages)
    {
        scene().getResource<ReservedInbox>().messages = std::move(messages);
        mirrors.update(scene());
        scene().getResource<ReservedInbox>().messages.clear();
        return scene().view<Mirror>().size();
    }
};
} // namespace

TEST_CASE(replicatedEntityIsMirrored)
{
    NetApp server = makeServer(addBoxPrefab);
    Entity box =
        server.scene().getResource<PrefabsManager>().InstantiatePrefab(BOX, server.scene());
    server.scene().getComponent<Position>(box).x = 3.0f;
    NetApp client = connectedClient(server, addBoxPrefab);
    Scene &scene = client.scene();
    CHECK(pollUntil({&server, &client}, [&] {
        auto mirrors = scene.view<Mirror>();
        return mirrors.size() == 1 && scene.getComponent<Position>(mirrors.front()).x == 3.0f;
    }));
    CHECK(scene.getComponent<Mirror>(scene.view<Mirror>().front()).netId ==
          server.scene().getComponent<NetId>(box).value);
}

TEST_CASE(despawnWinsOverLostAndLateStates)
{
    InjectedClient client;
    CHECK(client.inject({entityState(10)}) == 1);
    // The despawns of ticks 10 and 12 were lost
    CHECK(client.inject({despawn(14)}) == 0);
    CHECK(client.inject({entityState(12)}) == 0);
}

TEST_CASE(staleMirrorDespawns)
{
    constexpr std::uint64_t START_TICK = 100;
    InjectedClient client;
    auto &clock = client.scene().getResource<GameClock>();
    clock.tick = START_TICK;
    client.inject({entityState(10)});
    clock.tick += MIRROR_STALE_TICKS;
    CHECK(client.inject({}) == 1);
    ++clock.tick;
    CHECK(client.inject({}) == 0);
}
