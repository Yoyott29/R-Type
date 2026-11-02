/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Position
*/

#pragma once
#include "ComponentDescriptor.hpp"

struct Position {
    float x;
    float y;
};

template <> struct ComponentDescription<Position> {
    static constexpr bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Position",
                {
                    {"PositionX", valueType::Float, "0"},
                    {"PositionY", valueType::Float, "0"},
                },
                {58, 53, 63, 255},
                ""};
    }
};
