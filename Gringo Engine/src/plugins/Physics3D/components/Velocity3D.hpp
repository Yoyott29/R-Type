/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Velocity3D
*/

#pragma once
#include "ComponentDescriptor.hpp"

struct Velocity3D {
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
};

template <> struct ComponentDescription<Velocity3D> {
    static inline const bool isDescribed = true;
    static ComponentDescriptor describe()
    {
        return {"Velocity3D",
                {{"Dx", valueType::Float, "0"}, {"Dy", valueType::Float, "0"},
                 {"Dz", valueType::Float, "0"}},
                {68, 61, 56, 255}, "Physics3D"};
    }
};
