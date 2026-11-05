/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** SpriteSystem
*/

#pragma once
#include "IRenderer.hpp"
#include "Transform.hpp"
#include "Scene.hpp"
#include "Sprite.hpp"
#include "System.hpp"

// Runs in the Render schedule: draws every Sprite through the renderer, by drawOrder
class SpriteSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    // A texture fills the shape's size, otherwise the shape is drawn in its color
    static void drawSprite(IRenderer &renderer, Transform const &position, Sprite const &sprite);
};
