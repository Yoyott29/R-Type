/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Structs
*/

#pragma once
#include <string>
#include <functional>

struct Position {
    float x;
    float y;
};

struct ColorRGBA {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct Text {
    Position position;
    int fontSize;
    std::string text;
};

struct Button {
    Position position;
    int width;
    int height;
    Text text;
    std::function<void()> onClick;
};

