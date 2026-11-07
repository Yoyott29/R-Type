/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ArcWall3D
*/

#pragma once
#include "ComponentDescriptor.hpp"

// A vertical wall bent along a circle around the entity's position, for the turns of a track:
// one wall instead of many boxes. It only acts on the angles from startAngle to endAngle
// (degrees on the ground, atan2(z, x) from the centre, going from +x to +z), and on the
// heights of the entity's y +/- height / 2. One sided: keepInside keeps the moving bodies
// inside the circle (the outer wall of a turn), otherwise outside of it (the inner wall), so
// nothing goes through it however fast it moves
struct ArcWall3D {
    float radius = 10.0f;
    float startAngle = 0.0f;
    float endAngle = 90.0f;
    float height = 10.0f;
    bool keepInside = true;
};

template <> struct ComponentDescription<ArcWall3D> {
    static inline const bool isDescribed = true;
    static ComponentDescriptor describe()
    {
        return {"ArcWall3D",
                {{"Radius", valueType::Float, "10"},
                 {"StartAngle", valueType::Float, "0"},
                 {"EndAngle", valueType::Float, "90"},
                 {"Height", valueType::Float, "10"},
                 {"KeepInside", valueType::Bool, "True"}},
                {58, 57, 61, 255}, "Physics3D"};
    }
};
