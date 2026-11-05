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
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Sprite",
                {
                    {"Shape", valueType::Choice, "Box", {"Box", "Circle"}},
                    {"Width", valueType::Float, "32", {}, {"Shape", "Box"}},
                    {"Height", valueType::Float, "32", {}, {"Shape", "Box"}},
                    {"Radius", valueType::Float, "16", {}, {"Shape", "Circle"}},
                    {"Color", valueType::Color, "White"},
                    {"DrawOrder", valueType::Int, "0"},
                    {"TexturePath", valueType::File, NO_FILE, {"assets", ".png", ".jpg"}},
                },
                {51, 56, 70, 255},
                ""};
    }
};
