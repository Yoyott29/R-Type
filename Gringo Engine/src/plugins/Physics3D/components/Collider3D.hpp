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
    // Center of the box from the entity's position: for models whose origin is not their middle
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float offsetZ = 0.0f;
    std::function<void(Entity, Entity)> onCollision;
    std::uint32_t layer = 1;
    std::uint32_t mask = 0xFFFFFFFF;
    bool isTrigger = false;
    // Turns with the entity's Transform the way a 3D model drawn with it turns, offset included.
    // Off: the box stays lined up with the world axes. A turned box that isn't a trigger pushes
    // as the world-aligned box around it
    bool rotates = false;
};

template <> struct ComponentDescription<Collider3D> {
    static inline const bool isDescribed = true;
    static ComponentDescriptor describe()
    {
        return {"Collider3D",
                {{"Width", valueType::Float, "1"},
                 {"Height", valueType::Float, "1"},
                 {"Depth", valueType::Float, "1"},
                 {"OffsetX", valueType::Float, "0"},
                 {"OffsetY", valueType::Float, "0"},
                 {"OffsetZ", valueType::Float, "0"},
                 {"Layer", valueType::Int, "1"},
                 {"TypeCollisions", valueType::String, "All layers"},
                 {"IsTrigger", valueType::Bool, "False"},
                 {"Rotates", valueType::Bool, "False"}},
                {58, 57, 61, 255},
                "Physics3D"};
    }
};
