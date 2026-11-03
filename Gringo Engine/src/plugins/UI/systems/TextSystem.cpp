/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** TextSystem
*/

#include "TextSystem.hpp"
#include "IRenderer.hpp"
#include "Position.hpp"
#include "UIText.hpp"
#include <functional>

void TextSystem::update(Scene &scene)
{
    // Without the renderer plugin there is no window to draw in
    if (!scene.hasResource<std::reference_wrapper<IRenderer>>())
        return;
    IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();

    for (Entity entity : _entities) {
        Position const &position = scene.getComponent<Position>(entity);
        UIText const &text = scene.getComponent<UIText>(entity);
        renderer.drawText(position.x, position.y, text.content, text.fontSize, text.color);
    }
}
