/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Transform
*/

#pragma once
#include "ComponentDescriptor.hpp"

// Where an entity is in the world: position in pixels (z for depth, unused in 2D), scale (1: the
// size of its Sprite / Collider2D) and rotation in degrees
struct Transform {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float rotation = 0.0f;
};

template <> struct ComponentDescription<Transform> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Transform",
                {
                    {"PositionX", valueType::Float, "0"},
                    {"PositionY", valueType::Float, "0"},
                    {"PositionZ", valueType::Float, "0"},
                    {"ScaleX", valueType::Float, "1"},
                    {"ScaleY", valueType::Float, "1"},
                    {"Rotation", valueType::Float, "0"},
                },
                {58, 53, 63, 255},
                ""};
    }
};
