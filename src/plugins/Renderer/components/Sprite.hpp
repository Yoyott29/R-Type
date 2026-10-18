/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Sprite
*/

#pragma once
#include "Shape.hpp"
#include <variant>

struct ColorRGBA {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct Sprite {
    std::variant<BoxShape, CircleShape> shape;
    ColorRGBA color;
    int drawOrder = 0;
};
