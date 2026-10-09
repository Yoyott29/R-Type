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

        renderer.drawRectangle(position.x, position.y,
            sprite.width, sprite.height, sprite.color);
    }
}
