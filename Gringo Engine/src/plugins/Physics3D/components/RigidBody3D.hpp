/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RigidBody3D
*/

#pragma once
#include "ComponentDescriptor.hpp"

struct RigidBody3D {
    float mass = 1.0f;
    bool isStatic = false;
    float gravityScale = 1.0f;
    bool grounded = false;
};

template <> struct ComponentDescription<RigidBody3D> {
    static inline const bool isDescribed = true;
    static ComponentDescriptor describe()
    {
        return {"RigidBody3D",
                {{"Mass", valueType::Float, "1"}, {"IsStatic", valueType::Bool, "False"},
                 {"GravityScale", valueType::Float, "1"}},
                {63, 55, 51, 255}, "Physics3D"};
    }
};
