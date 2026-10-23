/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RTypeServerPlugin
*/

#include "RTypeServerPlugin.hpp"
#include "Collider2D.hpp"
#include "CollisionLayers.hpp"
#include "DestroyOffScreenScript.hpp"
#include "EnemyScript.hpp"
#include "EnemySpawnerScript.hpp"
#include "NetTransport.hpp"
#include "PlayerId.hpp"
#include "PlayerInputSystem.hpp"
#include "Position.hpp"
#include "PrefabsManager.hpp"
#include "ScriptSystem.hpp"
#include "ServerNetSystem.hpp"
#include "ServerPlayers.hpp"
#include "SnapshotSystem.hpp"
#include "SpaceSpawnerScript.hpp"
#include "Sprite.hpp"
#include "Type.hpp"
#include "Velocity.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

void eliminatePlayer(Scene &scene, PeerId peer)
{
    auto &players = scene.getResource<ServerPlayers>();
    auto it = players.byPeer.find(peer);
    if (it == players.byPeer.end())
        return;
    std::cout << "Player " << static_cast<int>(it->second.slot) << " eliminated" << '\n'
              << std::flush;
    scene.getResource<NetTransport>().disconnect(peer);
    scene.queueDestroy(it->second.entity);
    players.byPeer.erase(it);
}

namespace
{
void createPrefabs(Scene &scene, PrefabsManager &prefabs)
{
    prefabs.createPrefab(
        "Enemy", Prefab()
                     .addComponent(Type{EntType::ENEMY})
                     .addComponent(Position{2000.0f, 0.0f})
                     .addComponent(Velocity{-500.0f, 0.0f})
                     .addComponent(Sprite{BoxShape{40.0f, 60.0f}, ColorRGBA{228, 48, 0, 255}, 0,
                                          "Assets/Bydos1.png"})
                     .addComponent(Collider2D{BoxShape{40.0f, 60.0f}, nullptr, rtype_layers::ENEMY,
                                              rtype_layers::PLAYER | rtype_layers::PLAYER_BULLET})
                     .addComponent(Script{std::make_shared<DestroyOffScreenScript>()})
                     .addComponent(Script{std::make_shared<EnemyScript>()}));
    prefabs.createPrefab(
        "Enemy2", Prefab()
                      .addComponent(Type{EntType::ENEMY})
                      .addComponent(Position{2000.0f, 0.0f})
                      .addComponent(Velocity{-500.0f, 0.0f})
                      .addComponent(Sprite{BoxShape{40.0f, 60.0f}, ColorRGBA{228, 48, 0, 255}, 0,
                                           "Assets/Bydos2.png"})
                      .addComponent(Collider2D{BoxShape{40.0f, 60.0f}, nullptr, rtype_layers::ENEMY,
                                               rtype_layers::PLAYER | rtype_layers::PLAYER_BULLET})
                      .addComponent(Script{std::make_shared<DestroyOffScreenScript>()})
                      .addComponent(Script{std::make_shared<EnemyScript>()}));
    prefabs.createPrefab(
        "PlayerBullet", Prefab()
                            .addComponent(Type{EntType::BULLET})
                            .addComponent(Position{0.0f, 0.0f})
                            .addComponent(Velocity{550.0f, 0.0f})
                            .addComponent(Sprite{BoxShape{20.0f, 10.0f}, ColorRGBA{0, 0, 228, 255}})
                            .addComponent(Collider2D{
                                BoxShape{20.0f, 10.0f},
                                [&scene](Entity bullet, Entity other) {
                                    if (scene.isEntityAlive(other) &&
                                        scene.getComponent<Type>(other).type == EntType::ENEMY) {
                                        scene.queueDestroy(bullet);
                                        scene.queueDestroy(other);
                                    }
                                },
                                rtype_layers::PLAYER_BULLET, rtype_layers::ENEMY})
                            .addComponent(Script{std::make_shared<DestroyOffScreenScript>()}));
    prefabs.createPrefab(
        "Star", Prefab()
                    .addComponent(Type{EntType::OTHER})
                    .addComponent(Position{2000.0f, 0.0f})
                    .addComponent(Velocity{-100.0f, 0.0f})
                    .addComponent(Sprite{BoxShape{5.0f, 5.0f}, ColorRGBA{255, 255, 255, 255}, -1})
                    .addComponent(Script{std::make_shared<DestroyOffScreenScript>()}));
}

void createInitialEntities(Scene &scene, PrefabsManager &prefabs)
{
    Entity spawner = scene.createEntity();
    scene.addComponent(spawner, Script{std::make_shared<EnemySpawnerScript>(prefabs)});
    Entity spaceSpawner = scene.createEntity();
    scene.addComponent(spaceSpawner, Script{std::make_shared<SpaceSpawnerScript>(prefabs)});
    for (int i = 0; i < 20; i++) {
        Entity star = prefabs.InstantiatePrefab("Star", scene);
        scene.getComponent<Position>(star).x = static_cast<float>(rand() % 1920);
        scene.getComponent<Position>(star).y = static_cast<float>(rand() % 1080);
    }
}
} // namespace

void RTypeServerPlugin::build(Application &app)
{
    Scene &scene = app._scene;
    scene.registerComponent<PlayerId>();
    scene.registerComponent<Sprite>();
    scene.registerComponent<Type>();
    auto &prefabs = scene.addResource<PrefabsManager>();
    scene.addResource<ServerTick>();
    scene.addResource<ServerPlayers>();

    createPrefabs(scene, prefabs);
    scene.registerSystem<ServerNetSystem>(PrepUpdate);

    scene.registerSystem<PlayerInputSystem>(FixedUpdate);
    scene.registerSystem<ScriptSystem>(FixedUpdate);
    ComponentSet scriptRequirements;
    scriptRequirements.set(scene.getComponentType<Script>());
    scene.setSystemComponentSet<ScriptSystem>(scriptRequirements);

    scene.registerSystem<SnapshotSystem>(Update);
    ComponentSet snapshotRequirements;
    snapshotRequirements.set(scene.getComponentType<Position>());
    snapshotRequirements.set(scene.getComponentType<Type>());
    snapshotRequirements.set(scene.getComponentType<Sprite>());
    scene.setSystemComponentSet<SnapshotSystem>(snapshotRequirements);

    createInitialEntities(scene, prefabs);
}

void RTypeServerPlugin::cleanup(Application &) { std::cout << "Closing server\n" << std::flush; }
