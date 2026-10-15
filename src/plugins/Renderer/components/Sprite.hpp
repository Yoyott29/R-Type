/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Sprite
*/

#pragma once
#include "Shape.hpp"
#include <raylib.h>
#include <variant>

struct Sprite {
    std::variant<BoxShape, CircleShape> shape;
    // int layer;
    // Sprite sprite;
    Color color;
};
