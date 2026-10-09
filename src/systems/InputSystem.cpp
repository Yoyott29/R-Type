/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** InputSystem
*/

#include "InputSystem.hpp"
#include "Velocity.hpp"
#include <raylib.h>

void InputSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Velocity &velocity = scene.getComponent<Velocity>(entity);

        velocity.dx = 0.0f;
        velocity.dy = 0.0f;
        if (IsKeyDown(KEY_RIGHT)) 
            velocity.dx = PLAYER_SPEED;
        if (IsKeyDown(KEY_LEFT)) 
            velocity.dx = -PLAYER_SPEED;
        if (IsKeyDown(KEY_UP)) 
            velocity.dy = -PLAYER_SPEED;
        if (IsKeyDown(KEY_DOWN)) 
            velocity.dy = PLAYER_SPEED;
    }
}
