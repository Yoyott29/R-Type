/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Type
*/

#pragma once
#include <cstdint>

enum EntType { PLAYER, ENEMY, BULLET, OBSTACLE, OTHER };

// What an entity is; the client maps it to its visuals
struct Type {
    EntType type;
    std::uint8_t variant = 0;
};
