/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** GravitySystem
*/

#include "GravitySystem.hpp"

void GravitySystem::update(Scene &scene)
{
    const Gravity2D &gravity = scene.getResource<Gravity2D>();
    const std::vector<Entity> entities(_entities.begin(), _entities.end());

    scene.parallelFor(entities, [&](Entity entity) {
        const RigidBody2D &body = scene.getComponent<RigidBody2D>(entity);
        if (body.isStatic || body.gravityScale == 0.0f)
            return;
        Velocity &velocity = scene.getComponent<Velocity>(entity);
        velocity.dx += gravity.x * body.gravityScale * FIXED_DELTA_TIME;
        velocity.dy += gravity.y * body.gravityScale * FIXED_DELTA_TIME;
    });
}
