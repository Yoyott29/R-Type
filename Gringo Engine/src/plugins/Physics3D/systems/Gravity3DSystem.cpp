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
    for (Entity entity : _entities) {
        const RigidBody3D &body = scene.getComponent<RigidBody3D>(entity);
        if (body.isStatic || body.gravityScale == 0.0f)
            continue;
        Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);
        velocity.dx += gravity.x * body.gravityScale * FIXED_DELTA_TIME;
        velocity.dy += gravity.y * body.gravityScale * FIXED_DELTA_TIME;
        velocity.dz += gravity.z * body.gravityScale * FIXED_DELTA_TIME;
    }
}
