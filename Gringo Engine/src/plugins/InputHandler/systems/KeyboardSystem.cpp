/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** KeyboardSystem
*/

#include "KeyboardSystem.hpp"
#include "IRenderer.hpp"
#include "KeyboardInput.hpp"
#include <cstddef>
#include <functional>

void KeyboardSystem::update(Scene &scene)
{
    const IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();
    KeyboardInput &keyboard = scene.getResource<KeyboardInput>();

    keyboard.pressed.reset();
    for (std::size_t index = 0; index < static_cast<std::size_t>(Key::Unknown); ++index)
        if (renderer.isKeyDown(static_cast<Key>(index)))
            keyboard.pressed.set(index);
}
