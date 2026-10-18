/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RenderSystem
*/

#include "RenderSystem.hpp"
#include <algorithm>
#include <functional>

void RenderSystem::update(Scene &scene)
{
    IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();
    std::vector<Entity> entities(_entities.begin(), _entities.end());
    std::stable_sort(entities.begin(), entities.end(), [&scene](Entity left, Entity right) {
        return scene.getComponent<Sprite>(left).drawOrder <
               scene.getComponent<Sprite>(right).drawOrder;
    });

    renderer.beginFrame();
    renderer.clearBackground();
    for (Entity entity : entities) {
            Position const &position = scene.getComponent<Position>(entity);
            Sprite const &sprite = scene.getComponent<Sprite>(entity);
            renderer.drawSprite(position, sprite);
    }
    renderer.endFrame();
}
