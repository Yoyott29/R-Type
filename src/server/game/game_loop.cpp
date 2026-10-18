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
#include "Time.hpp"
#include "Type.hpp"
#include "Velocity.hpp"
#include "rtype_protocol.hpp"
#include <algorithm>
#include <csignal>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

namespace
{
std::atomic<bool> stopRequested{false};

void requestStop(int) { stopRequested = true; }
} // namespace

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

    initializePrebabs();
    initializeEntities();
}

void GameLoop::initializeEntities()
{
    Entity spawner = _scene.createEntity();
    _scene.addComponent(spawner, Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &) {
                                            Entity enemy = _game.SpawnEnemy(scene);
                                            if (enemy != static_cast<Entity>(NULL_INDEX))
                                                _entities.push_back(enemy);
                                        }});
    Entity spaceSwpawner = _scene.createEntity();
    _scene.addComponent(spaceSwpawner, Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &) {
                                                  Entity space = _game.SpawnSpace(scene);
                                                  if (space != static_cast<Entity>(NULL_INDEX))
                                                      _entities.push_back(space);
                                              }});
    for (int i = 0; i < 20; i++) {
        Entity star = _prefabs.InstantiatePrefab("Star", _scene);
        _scene.getComponent<Position>(star).x = static_cast<float>(rand() % 1920);
        _scene.getComponent<Position>(star).y = static_cast<float>(rand() % 1080);
        _entities.push_back(star);
    }
}

void GameLoop::initializePrebabs()
{
    _prefabs.createPrefab(
        "Enemy", Prefab()
                     .addComponent(Type{EntType::ENEMY})
                     .addComponent(Position{2000.0f, 0.0f})
                     .addComponent(Velocity{-500.0f, 0.0f})
                     .addComponent(Sprite{BoxShape{40.0f, 60.0f},
                                          ColorRGBA{228, 48, 0, 255},
                                          0,
                                          "Assets/Bydos1.png"})
                     .addComponent(Collider2D{BoxShape{40.0f, 60.0f}, nullptr})
                     .addComponent(Script{Timing::ON_UPDATE,
                                          [this](Scene &scene, Entity &enemy) {
                                              _game.DestroyOffScreenEntity(scene, enemy);
                                          }})
                     .addComponent(Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &enemy) {
                                              _game.makeSinusoidalMovement(scene, enemy);
                                          }}));
    _prefabs.createPrefab(
        "Enemy2", Prefab()
                      .addComponent(Type{EntType::ENEMY})
                      .addComponent(Position{2000.0f, 0.0f})
                      .addComponent(Velocity{-500.0f, 0.0f})
                      .addComponent(Sprite{BoxShape{40.0f, 60.0f},
                                           ColorRGBA{228, 48, 0, 255},
                                           0,
                                           "Assets/Bydos2.png"})
                      .addComponent(Collider2D{BoxShape{40.0f, 60.0f}, nullptr})
                      .addComponent(Script{Timing::ON_UPDATE,
                                           [this](Scene &scene, Entity &enemy) {
                                               _game.DestroyOffScreenEntity(scene, enemy);
                                           }})
                      .addComponent(Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &enemy) {
                                               _game.makeSinusoidalMovement(scene, enemy);
                                           }}));
    _prefabs.createPrefab(
        "PlayerBullet",
        Prefab()
            .addComponent(Type{EntType::BULLET})
            .addComponent(Position{0.0f, 0.0f})
            .addComponent(Velocity{550.0f, 0.0f})
            .addComponent(Sprite{BoxShape{20.0f, 10.0f}, ColorRGBA{0, 0, 228, 255}})
            .addComponent(Collider2D{BoxShape{20.0f, 10.0f},
                                     [this](Entity bullet, Entity other) {
                                         if (_scene.isEntityAlive(other) &&
                                             _scene.getComponent<Type>(other).type ==
                                                 EntType::ENEMY) {
                                             _scene.queueDestroy(bullet);
                                             _scene.queueDestroy(other);
                                         }
                                     }})
            .addComponent(Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &bullet) {
                                     _game.DestroyOffScreenEntity(scene, bullet);
                                 }}));
    _prefabs.createPrefab(
        "Star", Prefab()
                    .addComponent(Type{EntType::OTHER})
                    .addComponent(Position{2000.0f, 0.0f})
                    .addComponent(Velocity{-100.0f, 0.0f})
                    .addComponent(Sprite{BoxShape{5.0f, 5.0f}, ColorRGBA{255, 255, 255, 255}, -1})
                    .addComponent(Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &space) {
                                             _game.DestroyOffScreenEntity(scene, space);
                                         }}));
}

void GameLoop::initializePlayer(uint8_t id)
{
    Entity playerEntity = _scene.createEntity();
    _scene.addComponent(playerEntity, Type{EntType::PLAYER});
    _scene.addComponent(playerEntity, PlayerId{id});
    _scene.addComponent(playerEntity, Position{100.0f, 200.0f + 100.0f * static_cast<float>(id)});
    _scene.addComponent(playerEntity, Velocity{0.0f, 0.0f});
    _scene.addComponent(playerEntity, Sprite{BoxShape{103.0f, 50.0f}, _playerColors[id]});
    _scene.addComponent(playerEntity,
                        Collider2D{BoxShape{103.0f, 50.0f}, [this](Entity a, Entity other) {
                                       if (!_scene.isEntityAlive(other) ||
                                           _scene.getComponent<Type>(other).type != EntType::ENEMY)
                                           return;
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
        uint8_t playerId = rtype_protocol::NO_PLAYER;
        if (_scene.getComponent<Type>(entity).type == EntType::PLAYER)
            playerId = _scene.getComponent<PlayerId>(entity).value;
        Sprite const &sprite = _scene.getComponent<Sprite>(entity);
        uint8_t variant = rtype_protocol::DEFAULT_VARIANT;
        if (sprite.texturePath == "Assets/Bydos2.png")
            variant = rtype_protocol::ENEMY_VARIANT_2;
        float width = 0.0f;
        float height = 0.0f;
        uint8_t shape = rtype_protocol::BOX_SHAPE;
        std::visit(
            [&](auto const &value) {
                using Shape = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<Shape, BoxShape>) {
                    width = value.width;
                    height = value.height;
                } else {
                    width = value.radius;
                    height = value.radius;
                    shape = rtype_protocol::CIRCLE_SHAPE;
                }
            },
            sprite.shape);
        snapshot.push_back({_scene.getComponent<Position>(entity),
                            _scene.getComponent<Type>(entity),
                            playerId,
                            variant,
                            width,
                            height,
                            shape,
                            static_cast<int8_t>(sprite.drawOrder),
                            {sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a}});
    }
    return snapshot;
}

// Thread jeu : attend le prochain tick, puis execute le plus ancien input en attente de chaque
// joueur
void GameLoop::run()
{
    TickTimeManager &ticks = _scene.getResource<TickTimeManager>();
    ticks.start();
    std::signal(SIGINT, requestStop);
    std::signal(SIGTERM, requestStop);

    while (_running && !stopRequested) {
        ticks.advance();
        std::this_thread::sleep_until(ticks.getNextTickTime());
        for (float &cooldown : _shootCooldowns)
            cooldown = std::max(0.0f, cooldown - FIXED_DELTA_TIME);

        _network.dropTimedOutPlayers();
        std::vector<uint8_t> connected = _network.getConnectedPlayerIds();
        for (Entity entity : _entities)
            if (_scene.getComponent<Type>(entity).type == EntType::PLAYER &&
                std::find(connected.begin(), connected.end(),
                          _scene.getComponent<PlayerId>(entity).value) == connected.end())
                _scene.queueDestroy(entity);

        // "spawn": any newly connected player gets a position
        for (uint8_t id : connected) {
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
    _network.stop();
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

    if (_shootCooldowns.at(playerId) <= 0.0f) {
        Entity bullet = _prefabs.InstantiatePrefab("PlayerBullet", _scene);
        Position &pos = _scene.getComponent<Position>(bullet);
        pos.x = _scene.getComponent<Position>(playerEntity).x + 50.0f;
        pos.y = _scene.getComponent<Position>(playerEntity).y;
        _entities.push_back(bullet);
        _shootCooldowns.at(playerId) = 0.2f;
    }
}

} // namespace rtype_server
