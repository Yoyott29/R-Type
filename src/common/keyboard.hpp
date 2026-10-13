/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** keyboard
*/
#pragma once
#include <cstdint>

enum class Key : uint16_t { Z, S, Q, D, Up, Down, Left, Right, Space, Unknown };

struct InputEvent {
    enum class Type : uint8_t { KeyDown, KeyUp, Quit } type;
    Key key = Key::Unknown;
};