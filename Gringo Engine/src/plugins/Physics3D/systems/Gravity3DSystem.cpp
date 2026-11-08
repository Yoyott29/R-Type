/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Gravity3DSystem
*/

#include "Gravity3DSystem.hpp"

void Gravity3DSystem::update(Scene &scene)
{
    const Gravity3D &gravity = scene.getResource<Gravity3D>();
    const std::vector<Entity> entities(_entities.begin(), _entities.end());

    scene.parallelFor(entities, [&](Entity entity) {
        const RigidBody3D &body = scene.getComponent<RigidBody3D>(entity);
        if (body.isStatic || body.gravityScale == 0.0f)
            return;
        Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);
        const float gravityScale = body.gravityScale * body.mass;
        velocity.dx += gravity.x * gravityScale * FIXED_DELTA_TIME;
        velocity.dy += gravity.y * gravityScale * FIXED_DELTA_TIME;
        velocity.dz += gravity.z * gravityScale * FIXED_DELTA_TIME;
    });
}
