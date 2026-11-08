/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Collider2D
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include "Entity.hpp"
#include "Shape.hpp"
#include <cstdint>
#include <functional>
#include <variant>

// layer is what the entity is, mask what it reacts to: a pair only interacts when mask &
// other.layer.
// isTrigger: the collider calls onCollision every step it touches another collider.
// Otherwise the collider is solid: when two solid colliders start touching, their RigidBody2D
// react (see RigidBody2D.hpp)
struct Collider2D {
    std::variant<BoxShape, CircleShape> shape;
    std::function<void(Entity, Entity)> onCollision;
    std::uint32_t layer = 1;
    std::uint32_t mask = 0xFFFFFFFF;
    bool isTrigger = false;
};

template <> struct ComponentDescription<Collider2D> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Collider2D",
                {
                    {"Shape", valueType::Choice, "Box", {"Box", "Circle"}},
                    {"Width", valueType::Float, "32", {}, {"Shape", "Box"}},
                    {"Height", valueType::Float, "32", {}, {"Shape", "Box"}},
                    {"Radius", valueType::Float, "16", {}, {"Shape", "Circle"}},
                    {"Layer", valueType::Int, "1"},
                    {"TypeCollisions", valueType::String, "All layers"},
                    {"IsTrigger", valueType::Bool, "False"},
                },
                {58, 57, 61, 255},
                ""};
    }
};
