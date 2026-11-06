/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Movement3DSystem
*/

#include "Movement3DSystem.hpp"

void Movement3DSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Transform &position = scene.getComponent<Transform>(entity);
        const Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);
        position.x += velocity.dx * FIXED_DELTA_TIME;
        position.y += velocity.dy * FIXED_DELTA_TIME;
        position.z += velocity.dz * FIXED_DELTA_TIME;
    }
}
