/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** game_loop
*/

#include "game/game_loop.hpp"
#include "common/protocol.hpp"
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
        bool received = false;
        for (uint8_t playerId = 0; playerId < rtype_protocol::MAX_PLAYERS; playerId++) {
            auto &queue = _network.getPlayerInputs().at(playerId);
            std::optional<uint8_t> input = queue.pop();
            if (!input)
                continue;
            received = true;
            executeInput(playerId, *input, _ticks.getCurrentTick());
        }
        if (!received && _ticks.getCurrentTick() % 60 == 0)
            std::cout << "[tick " << _ticks.getCurrentTick() << "] nothing received" << std::endl;
    }
}

// Pour l'instant on affiche juste les touches, plus tard -> l'ECS ici
void GameLoop::executeInput(uint8_t playerId, uint8_t input, std::uint64_t tick)
{
    std::cout << "[tick " << tick << "] player " << static_cast<int>(playerId) << " :";
    debugPrintInput(input);
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
