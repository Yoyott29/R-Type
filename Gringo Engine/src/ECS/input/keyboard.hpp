/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** keyboard
*/

#pragma once
#include <cstdint>

enum class Key : uint16_t { Z, S, Q, D, Up, Down, Left, Right, Space, Unknown };

enum class Mouse : uint8_t { Left, Right, Middle };

// A point in world coordinates (the size given to openWindow), not window pixels
struct MousePosition {
    float x;
    float y;
};

struct InputEvent {
    enum class Type : uint8_t { KeyDown, KeyUp, Quit } type;
    Key key = Key::Unknown;
};
