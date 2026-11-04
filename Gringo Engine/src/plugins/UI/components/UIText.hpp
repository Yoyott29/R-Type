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
                },
                {60, 60, 75, 255},
                ""};
    }
};
