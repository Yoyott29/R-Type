/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Velocity
*/

#pragma once
#include "ComponentDescriptor.hpp"

struct Velocity {
    float dx;
    float dy;
};

template <> struct ComponentDescription<Velocity> {
    static constexpr bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Velocity",
                {
                    {"Dx", valueType::Float, "0"},
                    {"Dy", valueType::Float, "0"},
                },
                {51, 59, 63, 255}};
    }
};
