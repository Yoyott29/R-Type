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

    for (Entity entity : _entities) {
        const RigidBody2D &body = scene.getComponent<RigidBody2D>(entity);
        if (body.isStatic || body.gravityScale == 0.0f)
            continue;
        Velocity &velocity = scene.getComponent<Velocity>(entity);
        velocity.dx += gravity.x * body.gravityScale * FIXED_DELTA_TIME;
        velocity.dy += gravity.y * body.gravityScale * FIXED_DELTA_TIME;
    }
}
