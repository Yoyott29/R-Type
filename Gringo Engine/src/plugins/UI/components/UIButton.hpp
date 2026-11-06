/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UIButton
*/

#pragma once
#include "ColorRGBA.hpp"
#include "ComponentDescriptor.hpp"
#include "IGringoButtonScript.hpp"
#include <memory>
#include <string>

struct UIButton {
    std::string content;
    int width = 100;
    int height = 50;
    int fontSize = 24;
    ColorRGBA color{255, 255, 255, 255};
    ColorRGBA textColor{0, 0, 0, 255};
    // Called on click. Created by the game's generated main from the "Script" property
    std::shared_ptr<IGringoButtonScript> script;
    // The .ttf / .otf in the project's assets, resolved; empty: the renderer's built-in font
    std::string fontPath;
    // The image in the project's assets, resolved, drawn instead of the color; empty: none
    std::string imagePath;
};

template <> struct ComponentDescription<UIButton> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Button",
                {
                    {"Content", valueType::String, "Text"},
                    {"Width", valueType::Int, "100"},
                    {"Height", valueType::Int, "50"},
                    {"FontSize", valueType::Int, "24"},
                    {"Color", valueType::Color, "White"},
                    {"TextColor", valueType::Color, "Black"},
                    {"Font", valueType::File, DEFAULT_FONT, {"assets", ".ttf", ".otf"}},
                    {"ImagePath", valueType::File, NO_FILE, {"assets", ".png", ".jpg"}},
                    {"Script", valueType::File, NO_FILE, {"scripts", ".hpp"}},
                },
                {60, 60, 75, 255},
                ""};
    }
};
