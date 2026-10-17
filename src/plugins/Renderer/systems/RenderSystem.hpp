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

        // holds_alternative is a function that checks if a std::variant holds a specific type.
        if (std::holds_alternative<BoxShape>(sprite.shape)) {
          BoxShape const &box = std::get<BoxShape>(sprite.shape);
          DrawRectangleV({position.x - box.width / 2.0f, position.y - box.height / 2.0f},
            {box.width, box.height}, sprite.color);
        } else {
          CircleShape const &circle = std::get<CircleShape>(sprite.shape);
          DrawCircleV({position.x, position.y}, circle.radius, sprite.color);
        }
      }
      EndDrawing();
    }
};
