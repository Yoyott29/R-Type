/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** KeyboardSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Runs in PrepUpdate: asks the renderer which keys are down and stores them in KeyboardInput
class KeyboardSystem : public System
{
  public:
    void update(Scene &scene) override;
};
