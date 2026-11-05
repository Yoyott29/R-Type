/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** MovementSystem
*/

#include "MovementSystem.hpp"

void MovementSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Transform &position = scene.getComponent<Transform>(entity);
        Velocity const &velocity = scene.getComponent<Velocity>(entity);

        position.x += velocity.dx * FIXED_DELTA_TIME;
        position.y += velocity.dy * FIXED_DELTA_TIME;
    }
}
