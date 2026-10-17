/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** game_loop
*/

#pragma once
#include "Scene.hpp"
#include "network/network.hpp"
#include <atomic>
#include <cstdint>
#include <map>
#include <vector>

#define PLAYER_SPEED 3000.0f

namespace rtype_server
{
class GameLoop
{
  public:
    GameLoop(Network &network, std::atomic<bool> &running);
    void run();

  private:
    void executeInput(uint8_t playerId, uint8_t input, std::uint64_t tick);
    std::vector<rtype_protocol::EntitySnapshot> buildSnapshot();
    void debugPrintInput(uint8_t input); // DEBUG

    Network &_network;
    std::atomic<bool> &_running;
    Scene _scene;
    std::map<uint8_t, Entity> _players;
};
} // namespace rtype_server
