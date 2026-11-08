/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ArcWall3D
*/

#pragma once
#include "ComponentDescriptor.hpp"

// A vertical wall bent along a circle around the entity's position: one wall instead of many
// boxes for a curve, or a straight wall at any angle with a huge radius. It only acts on the angles
// from startAngle to endAngle (degrees on the ground, atan2(z, x) from the centre, going from +x to
// +z), and on the heights of the entity's y +/- height / 2. One sided: keepInside keeps the moving
// bodies inside the circle (the outer wall of a turn), otherwise outside of it (the inner wall), so
// nothing goes through it however fast it moves. reach: a body more than this far through the
// wall is left alone, it is on the other side of the circle (0: no limit)
struct ArcWall3D {
    float radius = 10.0f;
    float startAngle = 0.0f;
    float endAngle = 90.0f;
    float height = 10.0f;
    bool keepInside = true;
    float reach = 0.0f;
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
                 {"KeepInside", valueType::Bool, "True"},
                 {"Reach", valueType::Float, "0"}},
                {58, 57, 61, 255},
                "Physics3D"};
    }
};
