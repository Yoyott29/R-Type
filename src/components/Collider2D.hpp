/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Collider2D
*/

#pragma once
#include "Entity.hpp"
#include "Shape.hpp"
#include <functional>
#include <variant>

struct Collider2D {
    std::variant<BoxShape, CircleShape> shape;
    std::function<void(Entity, Entity)> onCollision;
};
