/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** TextSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Runs in the Overlay schedule: draws every UIText through the renderer, after the world
class TextSystem : public System
{
  public:
    void update(Scene &scene) override;
};
