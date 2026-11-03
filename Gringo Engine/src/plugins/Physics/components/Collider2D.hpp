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

// layer is what the entity is, mask what it reacts to: onCollision fires only when mask &
// other.layer
struct Collider2D {
    std::variant<BoxShape, CircleShape> shape;
    std::function<void(Entity, Entity)> onCollision;
    std::uint32_t layer = 1;
    std::uint32_t mask = 0xFFFFFFFF;
};

template <> struct ComponentDescription<Collider2D> {
    static constexpr bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Collider2D",
                {
                    {"Shape", valueType::Choice, "Box", {"Box", "Circle"}},
                    {"Width", valueType::Float, "32"},
                    {"Height", valueType::Float, "32"},
                    {"Radius", valueType::Float, "16"},
                    {"Layer", valueType::Int, "1"},
                    {"TypeCollisions", valueType::String, "All layers"},
                },
                {58, 57, 61, 255},
                ""};
    }
};
