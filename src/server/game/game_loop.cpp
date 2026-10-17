/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** game_loop
*/

#include "game/game_loop.hpp"
#include "rtype_protocol.hpp"
#include <iostream>
#include <optional>
#include <thread>

namespace rtype_server
{
GameLoop::GameLoop(Network &network, std::atomic<bool> &running)
    : _network(network), _running(running)
{
}

// Thread jeu : attend le prochain tick, puis execute le plus ancien input en attente de chaque
// joueur
void GameLoop::run()
{
    _ticks.start();

    while (_running) {
        _ticks.advance();
        std::this_thread::sleep_until(_ticks.getNextTickTime());

        // "spawn": any newly connected player gets a position
        for (uint8_t id : _network.getConnectedPlayerIds())
            _players.try_emplace(id, Position{100.0f, 200.0f + 100.0f * id});

        for (uint8_t playerId = 0; playerId < rtype_protocol::MAX_PLAYERS; playerId++) {
            auto &queue = _network.getPlayerInputs().at(playerId);
            std::optional<uint8_t> input = queue.pop();
            if (!input)
                continue;
            executeInput(playerId, *input, _ticks.getCurrentTick());
        }

        std::vector<rtype_protocol::EntitySnapshot> entities;
        for (auto const &[id, pos] : _players)
            entities.push_back({pos, Type{EntType::PLAYER}});
        _network.sendSnapshot(_ticks.getCurrentTick(), entities);
    }
}

void GameLoop::executeInput(uint8_t playerId, uint8_t input, std::uint64_t)
{
    auto it = _players.find(playerId);
    if (it == _players.end())
        return;
    float speed = PLAYER_SPEED * FIXED_DELTA_TIME;
    if (input & rtype_protocol::UP)
        it->second.y -= speed;
    if (input & rtype_protocol::DOWN)
        it->second.y += speed;
    if (input & rtype_protocol::LEFT)
        it->second.x -= speed;
    if (input & rtype_protocol::RIGHT)
        it->second.x += speed;
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
