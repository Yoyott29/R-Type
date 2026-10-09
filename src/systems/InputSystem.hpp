/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** InputSystem
*/

#pragma once
#include "System.hpp"
#include "Scene.hpp"
#define PLAYER_SPEED 150.0f

class InputSystem : public System {
    public:
        void update(Scene &scene);
};
