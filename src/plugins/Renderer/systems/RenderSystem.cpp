/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RenderSystem
*/

#include "RenderSystem.hpp"

void RenderSystem::update(Scene &scene)
{
    BeginDrawing();
    ClearBackground({0, 0, 0, 255});
    for (Entity entity : _entities) {
        Position const &position = scene.getComponent<Position>(entity);
        Sprite const &sprite = scene.getComponent<Sprite>(entity);

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
