/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Collider2D
*/

#pragma once
#include "Entity.hpp"
#include "Shape.hpp"
#include <cstdint>
#include <functional>
#include <variant>

// layer is what the entity is, mask what it reacts to: onCollision fires only when mask &
// other.layer
struct Collider2D {
    std::variant<BoxShape, CircleShape> shape;
    std::function<void(Entity, Entity)> onCollision;
    std::uint32_t layer = 1;
    std::uint32_t mask = 0xFFFFFFFF;
};
