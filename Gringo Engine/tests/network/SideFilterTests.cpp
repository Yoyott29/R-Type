#include "Check.hpp"
#include "Loopback.hpp"
#include "NetComponents.hpp"
#include "Networked.hpp"

namespace
{
Entity spawn(Scene &scene, NetMode mode)
{
    Entity entity = scene.createEntity();
    Networked networked;
    networked.mode = mode;
    scene.addComponent(entity, networked);
    return entity;
}
} // namespace

TEST_CASE(serverDropsClientEntities)
{
    NetApp server = makeServer();
    Scene &scene = server.scene();
    Entity parent = spawn(scene, NetMode::Client);
    Entity replicated = spawn(scene, NetMode::Replicated);
    step(server);
    CHECK(!scene.isEntityAlive(parent));
    CHECK(scene.isEntityAlive(replicated));
}

TEST_CASE(clientKeepsClientEntitiesAndMirrors)
{
    NetApp client = makeNetApp(NetworkConfig::client("127.0.0.1", 1));
    Scene &scene = client.scene();
    Entity bare = spawn(scene, NetMode::Replicated);
    Entity local = spawn(scene, NetMode::Client);
    Entity mirror = spawn(scene, NetMode::Replicated);
    scene.addComponent(mirror, Mirror{7, 0});
    step(client);
    CHECK(!scene.isEntityAlive(bare));
    CHECK(scene.isEntityAlive(local) && scene.isEntityAlive(mirror));
}
