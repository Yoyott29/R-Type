/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RenderSystem
*/

#pragma once
#include "Sprite.hpp"
#include "Position.hpp"
#include "Scene.hpp"
#include "System.hpp"

class RenderSystem : public System
{
  public:
    void update(Scene &scene);
};
