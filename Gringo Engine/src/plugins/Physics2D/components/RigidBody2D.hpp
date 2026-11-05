/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** RigidBody2D
*/

#pragma once
#include "ComponentDescriptor.hpp"

// Makes an entity react to gravity and to solid (non trigger) collisions; its Velocity is the one
// changed. When two moving bodies start touching, their momentums (mass * velocity) are added
// together and both leave with the same velocity. While solid colliders touch, they are pushed out
// of each other (the lighter one moving the most) and stop moving into each other, so bodies can
// rest on a floor. A static body, or an entity without RigidBody2D, is never moved.
// gravityScale multiplies the Physics2D "GravityX" / "GravityY" settings (0: not pulled)
struct RigidBody2D {
    float mass = 1.0f;
    bool isStatic = false;
    float gravityScale = 1.0f;
};

template <> struct ComponentDescription<RigidBody2D> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"RigidBody2D",
                {
                    {"Mass", valueType::Float, "1"},
                    {"IsStatic", valueType::Bool, "False"},
                    {"GravityScale", valueType::Float, "1"},
                },
                {63, 55, 51, 255},
                ""};
    }
};
