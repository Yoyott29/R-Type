/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** MovementSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"
#include "Time.hpp"
#include "Position.hpp"
#include "Velocity.hpp"

class MovementSystem : public System
{
  public:
    void update(Scene &scene) {
        for (Entity entity : _entities) {
            Position &position = scene.getComponent<Position>(entity);
            Velocity const &velocity = scene.getComponent<Velocity>(entity);

            position.x += velocity.dx * FIXED_DELTA_TIME;
            position.y += velocity.dy * FIXED_DELTA_TIME;
        }
    }
};
