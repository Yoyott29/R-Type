/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UIText
*/

#pragma once
#include "ColorRGBA.hpp"
#include "ComponentDescriptor.hpp"
#include <string>

struct UIText {
    std::string content;
    int fontSize = 32;
    ColorRGBA color{255, 255, 255, 255};
    // The .ttf / .otf in the project's assets, resolved; empty: the renderer's built-in font
    std::string fontPath;
};

template <> struct ComponentDescription<UIText> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Text",
                {
                    {"Content", valueType::String, "Text"},
                    {"FontSize", valueType::Int, "32"},
                    {"Color", valueType::Color, "White"},
                    {"Font", valueType::File, DEFAULT_FONT, {"assets", ".ttf", ".otf"}},
                },
                {60, 60, 75, 255},
                ""};
    }
};
