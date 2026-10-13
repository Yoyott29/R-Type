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
GameLoop::GameLoop(InputQueue &inputs, std::atomic<bool> &running)
    : _inputs(inputs), _running(running)
{
}

// Thread jeu : a chaque tick, execute le plus ancien input en attente, ou rien si la queue est vide
void GameLoop::run()
{
    auto nextTick = std::chrono::steady_clock::now();

    while (_running) {
        nextTick += TICK_DURATION;
        std::this_thread::sleep_until(nextTick);
        std::optional<uint8_t> input = _inputs.pop();
        if (!input) {
            std::cout << "[tick] ----- nothing received" << std::endl;
            continue;
        }
        executeInput(*input);
    }
}

// Pour l'instant on affiche juste les touches, plus tard -> l'ECS ici
void GameLoop::executeInput(uint8_t input)
{
    std::cout << "[tick] executed :";
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
