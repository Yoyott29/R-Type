/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** PlayerInputSystem
*/

#include "PlayerInputSystem.hpp"
#include "Position.hpp"
#include "PrefabsManager.hpp"
#include "ServerPlayers.hpp"
#include "Time.hpp"
#include "Velocity.hpp"
#include <algorithm>

namespace
{
constexpr float PLAYER_SPEED = 400.0f;
constexpr float SHOOT_COOLDOWN = 0.2f;
constexpr float BULLET_OFFSET_X = 50.0f;
} // namespace

void PlayerInputSystem::update(Scene &scene)
{
    ++scene.getResource<ServerTick>().value;
    auto &players = scene.getResource<ServerPlayers>();

    for (auto &[peer, player] : players.byPeer) {
        player.shootCooldown = std::max(0.0f, player.shootCooldown - FIXED_DELTA_TIME);
        if (player.inputs.empty())
            continue;
        std::uint8_t const input = player.inputs.front();
        player.inputs.pop_front();

        Velocity &velocity = scene.getComponent<Velocity>(player.entity);
        velocity = {0.0f, 0.0f};
        if (input & rtype_protocol::UP)
            velocity.dy -= PLAYER_SPEED;
        if (input & rtype_protocol::DOWN)
            velocity.dy += PLAYER_SPEED;
        if (input & rtype_protocol::LEFT)
            velocity.dx -= PLAYER_SPEED;
        if (input & rtype_protocol::RIGHT)
            velocity.dx += PLAYER_SPEED;

        if (player.shootCooldown <= 0.0f) {
            Position const shipPosition = scene.getComponent<Position>(player.entity);
            Entity bullet =
                scene.getResource<PrefabsManager>().InstantiatePrefab("PlayerBullet", scene);
            Position &position = scene.getComponent<Position>(bullet);
            position.x = shipPosition.x + BULLET_OFFSET_X;
            position.y = shipPosition.y;
            player.shootCooldown = SHOOT_COOLDOWN;
        }
    }
}
