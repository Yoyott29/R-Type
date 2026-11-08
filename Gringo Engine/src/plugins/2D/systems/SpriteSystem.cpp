/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** SpriteSystem
*/

#include "SpriteSystem.hpp"
#include <algorithm>
#include <functional>
#include <variant>
#include <vector>

void SpriteSystem::update(Scene &scene)
{
    IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();
    std::vector<Entity> entities(_entities.begin(), _entities.end());
    std::stable_sort(entities.begin(), entities.end(), [&scene](Entity left, Entity right) {
        return scene.getComponent<Sprite>(left).drawOrder <
               scene.getComponent<Sprite>(right).drawOrder;
    });

    for (Entity entity : entities)
        drawSprite(renderer, scene.getComponent<Transform>(entity),
                   scene.getComponent<Sprite>(entity));
}

void SpriteSystem::drawSprite(IRenderer &renderer, Transform const &position, Sprite const &sprite)
{
    if (std::holds_alternative<BoxShape>(sprite.shape)) {
        BoxShape const &box = std::get<BoxShape>(sprite.shape);
        if (sprite.texturePath.empty())
            renderer.drawRectangle(position.x, position.y, box.width, box.height, sprite.color);
        else
            drawTexture(renderer, sprite, position.x, position.y, box.width, box.height);
        return;
    }
    CircleShape const &circle = std::get<CircleShape>(sprite.shape);
    if (sprite.texturePath.empty())
        renderer.drawCircle(position.x, position.y, circle.radius, sprite.color);
    else
        drawTexture(renderer, sprite, position.x, position.y, circle.radius * 2.0f,
                    circle.radius * 2.0f);
}

void SpriteSystem::drawTexture(IRenderer &renderer, Sprite const &sprite, float x, float y,
                               float width, float height)
{
    SpriteRegion const &region = sprite.region;
    if (region.width <= 0.0f || region.height <= 0.0f)
        renderer.drawImage(sprite.texturePath, x, y, width, height);
    else
        renderer.drawImageRegion(sprite.texturePath, x, y, width, height, region.x, region.y,
                                 region.width, region.height);
}
