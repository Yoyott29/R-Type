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
        const float width = static_cast<float>(button.width);
        const float height = static_cast<float>(button.height);
        if (button.imagePath.empty())
            renderer.drawRectangle(position.x, position.y, width, height, button.color);
        else
            renderer.drawImage(button.imagePath, position.x, position.y, width, height);
        // Every draw call is centered on (x, y), so the text lands in the middle of the button
        renderer.drawText(position.x, position.y, button.content, button.fontSize, button.textColor,
                          button.fontPath);
    }
}
