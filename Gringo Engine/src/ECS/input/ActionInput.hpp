/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ActionInput
*/

#pragma once
#include <bitset>
#include <cstddef>

// How many actions a game can have
inline constexpr std::size_t MAX_ACTIONS = 32;

// The game actions held this frame (move up, fire...), not keys: a game numbers its actions and
// maps its keys to them, so the same action can come from any key or any source
struct ActionInput {
    std::bitset<MAX_ACTIONS> active;

    // False for an action of MAX_ACTIONS or more
    bool isActive(std::size_t action) const { return action < MAX_ACTIONS && active.test(action); }
};
