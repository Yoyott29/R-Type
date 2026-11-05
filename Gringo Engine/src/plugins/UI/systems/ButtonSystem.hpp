/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ButtonSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Runs in the Overlay schedule: draws every UIButton through the renderer, after the world
class ButtonSystem : public System
{
  public:
    void update(Scene &scene) override;
};
