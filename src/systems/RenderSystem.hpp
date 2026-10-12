/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RenderSystem
*/

#pragma once
#include "IRenderer.hpp"
#include "Scene.hpp"
#include "System.hpp"

class RenderSystem : public System
{
  public:
    void update(Scene &scene, IRenderer &renderer);
};
