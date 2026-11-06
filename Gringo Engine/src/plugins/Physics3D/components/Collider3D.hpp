/*
** EPITECH PROJECT, 2026
** ù
** File description:
** Collider3D
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include "Entity.hpp"
#include <cstdint>
#include <functional>

struct Collider3D {
    float width = 1.0f;
    float height = 1.0f;
    float depth = 1.0f;
    std::function<void(Entity, Entity)> onCollision;
    std::uint32_t layer = 1;
    std::uint32_t mask = 0xFFFFFFFF;
    bool isTrigger = false;
};

template <> struct ComponentDescription<Collider3D> {
    static inline const bool isDescribed = true;
    static ComponentDescriptor describe()
    {
        return {"Collider3D",
                {{"Width", valueType::Float, "1"}, {"Height", valueType::Float, "1"},
                 {"Depth", valueType::Float, "1"}, {"Layer", valueType::Int, "1"},
                 {"TypeCollisions", valueType::String, "All layers"},
                 {"IsTrigger", valueType::Bool, "False"}},
                {58, 57, 61, 255}, "Physics3D"};
    }
};
