/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** game_loop
*/

#include "game/game_loop.hpp"
#include "MovementSystem.hpp"
#include "PlayerId.hpp"
#include "Position.hpp"

#include "Sprite.hpp"
#include "TickTimeManager.hpp"
#include "Type.hpp"
#include "Velocity.hpp"
#include "rtype_protocol.hpp"
#include <algorithm>
#include <iostream>
#include <optional>
#include <thread>

namespace rtype_server
{
GameLoop::GameLoop(Network &network, std::atomic<bool> &running)
    : _network(network), _running(running), _game(_prefabs)
{
    _scene.registerComponent<Position>();
    _scene.registerComponent<Velocity>();
    _scene.registerComponent<Type>();
    _scene.registerComponent<PlayerId>();
    _scene.registerComponent<Sprite>();
    _scene.registerComponent<Collider2D>();
    _scene.registerComponent<Script>();
    _scene.addResource<TickTimeManager>();

    _scene.registerSystem<MovementSystem>(FixedUpdate);
    ComponentSet movementRequirements;
    movementRequirements.set(_scene.getComponentType<Position>());
    movementRequirements.set(_scene.getComponentType<Velocity>());
    _scene.setSystemComponentSet<MovementSystem>(movementRequirements);

    _scene.registerSystem<ScriptSystem>(FixedUpdate);
    ComponentSet scriptRequirements;
    scriptRequirements.set(_scene.getComponentType<Script>());
    _scene.setSystemComponentSet<ScriptSystem>(scriptRequirements);

    _scene.registerSystem<Collider2DSystem>(FixedUpdate);
    ComponentSet colliderRequirements;
    colliderRequirements.set(_scene.getComponentType<Position>());
    colliderRequirements.set(_scene.getComponentType<Collider2D>());
    _scene.setSystemComponentSet<Collider2DSystem>(colliderRequirements);

    initializeEntities();
    initializePrebabs();
}

void GameLoop::initializeEntities()
{
    Entity spawner = _scene.createEntity();
    _scene.addComponent(spawner, Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &) {
                                            Entity enemy = _game.SpawnEnemy(scene);
                                            if (enemy != static_cast<Entity>(NULL_INDEX))
                                                _entities.push_back(enemy);
                                        }});
}

void GameLoop::initializePrebabs()
{
    _prefabs.createPrefab(
        "Enemy", Prefab()
                     .addComponent(Type{EntType::ENEMY})
                     .addComponent(Position{2000.0f, 0.0f})
                     .addComponent(Velocity{-300.0f, 0.0f})
                     .addComponent(Sprite{BoxShape{50.0f, 35.0f}, Color{228, 48, 0, 255}})
                     .addComponent(Collider2D{BoxShape{50.0f, 35.0f}, nullptr})
                     .addComponent(Script{Timing::ON_UPDATE,
                                          [this](Scene &scene, Entity &enemy) {
                                              _game.killOffScreenEnemy(scene, enemy);
                                          }})
                     .addComponent(Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &enemy) {
                                              _game.makeSinusoidalMovement(scene, enemy);
                                          }}));
}

void GameLoop::initializePlayer(uint8_t id)
{
    Entity playerEntity = _scene.createEntity();
    _scene.addComponent(playerEntity, Type{EntType::PLAYER});
    _scene.addComponent(playerEntity, PlayerId{id});
    _scene.addComponent(playerEntity, Position{100.0f, 200.0f + 100.0f * id});
    _scene.addComponent(playerEntity, Velocity{0.0f, 0.0f});
    _scene.addComponent(playerEntity, Sprite{BoxShape{50.0f, 35.0f}, Color{0, 228, 48, 255}});
    _scene.addComponent(playerEntity, Collider2D{BoxShape{50.0f, 35.0f}, [this](Entity a, Entity) {
                                                     const uint8_t playerId =
                                                         _scene.getComponent<PlayerId>(a).value;
                                                     _network.disconnectPlayer(playerId);
                                                     _scene.queueDestroy(a);
                                                 }});
    _scene.addComponent(playerEntity,
                        Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &player) {
                                   _game.maintainPlayerOnScreen(scene, player);
                               }});
    _entities.push_back(playerEntity);
}

std::vector<rtype_protocol::EntitySnapshot> GameLoop::buildSnapshot()
{
    std::vector<rtype_protocol::EntitySnapshot> snapshot;

    for (Entity entity : _entities) {
        if (!_scene.isEntityAlive(entity))
            continue;
        snapshot.push_back(
            {_scene.getComponent<Position>(entity), _scene.getComponent<Type>(entity)});
    }
    return snapshot;
}

// Thread jeu : attend le prochain tick, puis execute le plus ancien input en attente de chaque
// joueur
void GameLoop::run()
{
    TickTimeManager &ticks = _scene.getResource<TickTimeManager>();
    ticks.start();

    while (_running) {
        ticks.advance();
        std::this_thread::sleep_until(ticks.getNextTickTime());

        // "spawn": any newly connected player gets a position
        for (uint8_t id : _network.getConnectedPlayerIds()) {
            bool alreadySpawned = false;
            for (Entity entity : _entities) {
                if (_scene.isEntityAlive(entity) &&
                    _scene.getComponent<Type>(entity).type == EntType::PLAYER &&
                    _scene.getComponent<PlayerId>(entity).value == id) {
                    alreadySpawned = true;
                    break;
                }
            }
            if (alreadySpawned)
                continue;
            initializePlayer(id);
        }

        for (uint8_t playerId = 0; playerId < rtype_protocol::MAX_PLAYERS; playerId++) {
            auto &queue = _network.getPlayerInputs().at(playerId);
            std::optional<uint8_t> input = queue.pop();
            if (!input)
                continue;
            executeInput(playerId, *input, ticks.getCurrentTick());
        }
        _scene.runSchedule(FixedUpdate);
        _scene.flushDestroyQueue();
        _entities.erase(
            std::remove_if(_entities.begin(), _entities.end(),
                           [this](Entity entity) { return !_scene.isEntityAlive(entity); }),
            _entities.end());
        _network.sendSnapshot(ticks.getCurrentTick(), buildSnapshot());
    }
}

void GameLoop::executeInput(uint8_t playerId, uint8_t input, std::uint64_t)
{
    Entity playerEntity = static_cast<Entity>(NULL_INDEX);
    for (Entity entity : _entities) {
        if (_scene.isEntityAlive(entity) &&
            _scene.getComponent<Type>(entity).type == EntType::PLAYER &&
            _scene.getComponent<PlayerId>(entity).value == playerId) {
            playerEntity = entity;
            break;
        }
    }
    if (playerEntity == static_cast<Entity>(NULL_INDEX))
        return;
    Velocity &velocity = _scene.getComponent<Velocity>(playerEntity);
    velocity = {0.0f, 0.0f};
    if (input & rtype_protocol::UP)
        velocity.dy -= PLAYER_SPEED;
    if (input & rtype_protocol::DOWN)
        velocity.dy += PLAYER_SPEED;
    if (input & rtype_protocol::LEFT)
        velocity.dx -= PLAYER_SPEED;
    if (input & rtype_protocol::RIGHT)
        velocity.dx += PLAYER_SPEED;
}

} // namespace rtype_server
