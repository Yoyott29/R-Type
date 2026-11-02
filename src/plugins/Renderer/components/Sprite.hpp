/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Sprite
*/

#pragma once
#include "ColorRGBA.hpp"
#include "ComponentDescriptor.hpp"
#include "Shape.hpp"
#include <string>
#include <variant>

struct Sprite {
    std::variant<BoxShape, CircleShape> shape;
    ColorRGBA color;
    int drawOrder = 0;
    std::string texturePath = "";
};

template <> struct ComponentDescription<Sprite> {
    static constexpr bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Sprite",
                {
                    {"Shape", valueType::Shape, "Box"},
                    {"Width", valueType::Float, "32"},
                    {"Height", valueType::Float, "32"},
                    {"Radius", valueType::Float, "16"},
                    {"Color", valueType::Color, "White"},
                    {"DrawOrder", valueType::Int, "0"},
                    {"TexturePath", valueType::String, "default.png"},
                },
                {51, 56, 70, 255},
                ""};
    }
};
