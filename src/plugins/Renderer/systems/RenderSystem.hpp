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
    void update(Scene &scene) {
      BeginDrawing();
      ClearBackground({0, 0, 0, 255}); // black background
      for (Entity entity : _entities) {
        Position const &position = scene.getComponent<Position>(entity);
        Sprite const &sprite = scene.getComponent<Sprite>(entity);
        DrawRectangle(static_cast<int>(position.x), static_cast<int>(position.y),
          static_cast<int>(sprite.width), static_cast<int>(sprite.height), sprite.color);
      }
      EndDrawing();
    }
};
