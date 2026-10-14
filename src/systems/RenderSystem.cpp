/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RenderSystem
*/

#include "RenderSystem.hpp"
#include "Position.hpp"
#include "Sprite.hpp"

void RenderSystem::update(Scene &scene, IRenderer &renderer)
{
    for (Entity entity : _entities) {
        Position const &position = scene.getComponent<Position>(entity);
        Sprite const &sprite = scene.getComponent<Sprite>(entity);

        // holds_alternative is a function that checks if a std::variant holds a specific type.
        if (std::holds_alternative<BoxShape>(sprite.shape)) {
            BoxShape const &box = std::get<BoxShape>(sprite.shape);
            renderer.drawRectangle(position.x - box.width / 2.0f, position.y - box.height / 2.0f,
                                   box.width, box.height, sprite.color);
        } else {
            CircleShape const &circle = std::get<CircleShape>(sprite.shape);
            renderer.drawCircle(position.x, position.y, circle.radius, sprite.color);
        }
    }
}
