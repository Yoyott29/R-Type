/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** ButtonSystem
*/

#include "ButtonSystem.hpp"
#include "IRenderer.hpp"
#include "Transform.hpp"
#include "UIButton.hpp"
#include <functional>

void ButtonSystem::update(Scene &scene)
{
    // Without the renderer plugin there is no window to draw in
    if (!scene.hasResource<std::reference_wrapper<IRenderer>>())
        return;
    IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();

    for (Entity entity : _entities) {
        Transform const &position = scene.getComponent<Transform>(entity);
        UIButton const &button = scene.getComponent<UIButton>(entity);
        renderer.drawRectangle(position.x, position.y, button.width, button.height, button.color);
        // Every draw call is centered on (x, y), so the text lands in the middle of the button
        renderer.drawText(position.x, position.y, button.content, button.fontSize,
                          button.textColor);
    }
}
