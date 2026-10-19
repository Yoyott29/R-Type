/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** game_loop
*/

#pragma once
#include "Collider2D.hpp"
#include "Collider2DSystem.hpp"
#include "PlayerId.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "ScriptSystem.hpp"
#include "Sprite.hpp"
#include "network/network.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

#define PLAYER_SPEED 400.0f

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
    void initializePlayer(uint8_t id);
    void initializeEntities();
    void initializePrebabs();

    Network &_network;
    std::atomic<bool> &_running;
    Scene _scene;
    std::vector<Entity> _entities;
    std::array<float, rtype_protocol::MAX_PLAYERS> _shootCooldowns{};
    std::array<ColorRGBA, rtype_protocol::MAX_PLAYERS> _playerColors{
        ColorRGBA{0, 228, 48, 255}, ColorRGBA{0, 0, 255, 255}, ColorRGBA{255, 0, 255, 255},
        ColorRGBA{255, 0, 0, 255}};
    PrefabsManager _prefabs;
};
} // namespace rtype_server
