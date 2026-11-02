/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** KeyboardInput
*/

#pragma once
#include "keyboard.hpp"
#include <bitset>
#include <cstddef>

struct KeyboardInput {
    std::bitset<static_cast<std::size_t>(Key::Unknown)> pressed;

    bool isDown(Key key) const
    {
        return key != Key::Unknown && pressed.test(static_cast<std::size_t>(key));
    }
};
