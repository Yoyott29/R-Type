/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ButtonClickSystem
*/

#include "ButtonClickSystem.hpp"
#include "ButtonClicked.hpp"
#include "EventBus.hpp"
#include "IRenderer.hpp"
#include "Position.hpp"
#include "UIButton.hpp"
#include <cmath>
#include <functional>

void ButtonClickSystem::update(Scene &scene)
{
    // Without the renderer plugin there is no window, so no mouse
    if (!scene.hasResource<std::reference_wrapper<IRenderer>>())
        return;
    IRenderer const &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();
    if (!renderer.isMouseButtonPressed(Mouse::Left))
        return;

    MousePosition const mouse = renderer.getMousePosition();
    EventBus &bus = scene.getResource<EventBus>();
    for (Entity entity : _entities) {
        Position const &position = scene.getComponent<Position>(entity);
        UIButton const &button = scene.getComponent<UIButton>(entity);
        // A button is drawn centered on its position
        if (std::abs(mouse.x - position.x) <= static_cast<float>(button.width) / 2.0f &&
            std::abs(mouse.y - position.y) <= static_cast<float>(button.height) / 2.0f)
            bus.emit(ButtonClicked{entity});
    }
}
