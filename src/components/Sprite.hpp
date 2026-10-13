/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Sprite
*/

#pragma once
#include "ColorRGBA.hpp"
#include "Shape.hpp"
#include <variant>

struct Sprite {
    std::variant<BoxShape, CircleShape> shape;
    // int layer;
    // Sprite sprite;
    ColorRGBA color;
};
