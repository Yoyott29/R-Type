/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** server
*/

#pragma once
#include "game/game_loop.hpp"
#include "input/input_queue.hpp"
#include "network/network.hpp"
#include <atomic>
#include <cstdint>
#include <map>

namespace rtype_server
{
class Server
{
  public:
    explicit Server(uint16_t port, const std::string &ip);
    void run();

  private:
    std::atomic<bool> _running{false};
    // Les MAX_PLAYERS entrees sont creees dans le constructeur, puis on n'utilise que .at() :
    // la map ne change jamais de forme pendant que les deux threads la lisent
    std::map<uint8_t, InputQueue> _playerInputs; // playerId -> InputQueue
    Network _network;
    GameLoop _gameLoop;
};
} // namespace rtype_server
