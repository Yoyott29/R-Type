/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** PlayerInputSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Applies one buffered input per player per fixed tick
class PlayerInputSystem : public System
{
  public:
    void update(Scene &scene) override;
};
