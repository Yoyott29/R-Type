/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** CollisionLayers
*/

#pragma once
#include <cstdint>

// Collider2D layer/mask bits used by R-Type
namespace rtype_layers
{
const std::uint32_t PLAYER = 1 << 0;
const std::uint32_t ENEMY = 1 << 1;
const std::uint32_t PLAYER_BULLET = 1 << 2;
} // namespace rtype_layers
