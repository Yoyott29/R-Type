/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** game_loop
*/

#pragma once
#include "TickTimeManager.hpp"
#include "network/network.hpp"
#include <atomic>
#include <cstdint>

#define PLAYER_SPEED 300.0f

namespace rtype_server
{
class GameLoop
{
  public:
    GameLoop(Network &network, std::atomic<bool> &running);
    void run();

  private:
    void executeInput(uint8_t playerId, uint8_t input, std::uint64_t tick);
    void debugPrintInput(uint8_t input); // DEBUG

    Network &_network;
    TickTimeManager _ticks;
    std::atomic<bool> &_running;
    std::map<uint8_t, Position> _players;
};
} // namespace rtype_server
