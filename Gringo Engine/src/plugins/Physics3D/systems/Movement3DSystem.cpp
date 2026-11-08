/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Movement3DSystem
*/

#include "Movement3DSystem.hpp"

void Movement3DSystem::update(Scene &scene)
{
    const std::vector<Entity> entities(_entities.begin(), _entities.end());

    scene.parallelFor(entities, [&](Entity entity) {
        Transform &position = scene.getComponent<Transform>(entity);
        const Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);
        position.x += velocity.dx * FIXED_DELTA_TIME;
        position.y += velocity.dy * FIXED_DELTA_TIME;
        position.z += velocity.dz * FIXED_DELTA_TIME;
    });
}
