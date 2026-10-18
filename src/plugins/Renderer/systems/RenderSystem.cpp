/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RenderSystem
*/

#include "RenderSystem.hpp"
#include <functional>

void RenderSystem::update(Scene &scene)
{
    IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();

    renderer.beginFrame();
    renderer.clearBackground();
    for (Entity entity : _entities) {
        Position const &position = scene.getComponent<Position>(entity);
        Sprite const &sprite = scene.getComponent<Sprite>(entity);
        renderer.drawSprite(position, sprite);
    }
    renderer.endFrame();
}
