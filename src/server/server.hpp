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

namespace rtype_server
{
class Server
{
  public:
    static constexpr uint16_t DEFAULT_PORT = 1234;

    explicit Server(uint16_t port);
    void run();

  private:
    std::atomic<bool> _running{false};
    InputQueue _inputs;
    Network _network;
    GameLoop _gameLoop;
};
} // namespace rtype_server
