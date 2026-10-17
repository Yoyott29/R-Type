/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** game_loop
*/

#include "game/game_loop.hpp"
#include "MovementSystem.hpp"
#include "Position.hpp"

#include "Sprite.hpp"
#include "TickTimeManager.hpp"
#include "Type.hpp"
#include "Velocity.hpp"
#include "rtype_protocol.hpp"
#include <iostream>
#include <optional>
#include <thread>

namespace rtype_server
{
GameLoop::GameLoop(Network &network, std::atomic<bool> &running)
    : _network(network), _running(running)
{
    _scene.registerComponent<Position>();
    _scene.registerComponent<Velocity>();
    _scene.registerComponent<Type>();
    _scene.registerComponent<Sprite>();
    _scene.addResource<TickTimeManager>();

    _scene.registerSystem<MovementSystem>(FixedUpdate);
    ComponentSet movementRequirements;
    movementRequirements.set(_scene.getComponentType<Position>());
    movementRequirements.set(_scene.getComponentType<Velocity>());
    _scene.setSystemComponentSet<MovementSystem>(movementRequirements);
}

std::vector<rtype_protocol::EntitySnapshot> GameLoop::buildSnapshot()
{
    std::vector<rtype_protocol::EntitySnapshot> snapshot;

    for (auto const &[id, entity] : _players)
        snapshot.push_back({_scene.getComponent<Position>(entity), Type{EntType::PLAYER}});
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
            Entity playerEntity = _scene.createEntity();
            _scene.addComponent(playerEntity, Position{100.0f, 200.0f + 100.0f * id});
            _scene.addComponent(playerEntity, Velocity{0.0f, 0.0f});
            _scene.addComponent(playerEntity,
                                Sprite{BoxShape{50.0f, 35.0f}, Color{0, 228, 48, 255}});
            _players.emplace(id, playerEntity);
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
        _network.sendSnapshot(ticks.getCurrentTick(), buildSnapshot());
    }
}

void GameLoop::executeInput(uint8_t playerId, uint8_t input, std::uint64_t)
{
    auto it = _players.find(playerId);
    if (it == _players.end())
        return;
    Velocity &velocity = _scene.getComponent<Velocity>(it->second);
    velocity = {0.0f, 0.0f};
    float speed = PLAYER_SPEED * FIXED_DELTA_TIME;
    if (input & rtype_protocol::UP)
        velocity.dy -= speed;
    if (input & rtype_protocol::DOWN)
        velocity.dy += speed;
    if (input & rtype_protocol::LEFT)
        velocity.dx -= speed;
    if (input & rtype_protocol::RIGHT)
        velocity.dx += speed;
}
// DEBUG
void GameLoop::debugPrintInput(uint8_t input)
{
    if (input == 0)
        std::cout << " NONE";
    if (input & rtype_protocol::UP)
        std::cout << " UP";
    if (input & rtype_protocol::DOWN)
        std::cout << " DOWN";
    if (input & rtype_protocol::LEFT)
        std::cout << " LEFT";
    if (input & rtype_protocol::RIGHT)
        std::cout << " RIGHT";
    if (input & rtype_protocol::CHARGE)
        std::cout << " CHARGE";
    std::cout << " (" << static_cast<int>(input) << ")" << std::endl;
}
} // namespace rtype_server
