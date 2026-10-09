/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** MovementSystem
*/

#pragma once
#include "System.hpp"
#include "Scene.hpp"
#include "Time.hpp"

class MovementSystem : public System {
    public:
        void update(Scene &scene);
};
