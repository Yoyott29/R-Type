/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** InputHandlerPlugin
*/

#include "InputHandlerPlugin.hpp"
#include "KeyboardInput.hpp"
#include "KeyboardSystem.hpp"

void InputHandlerPlugin::build(Application &app)
{
    // The keyboard is read at the start of every frame, before the game logic
    app._scene.addResource<KeyboardInput>();
    app._scene.registerSystem<KeyboardSystem>(PrepUpdate);
}

void InputHandlerPlugin::cleanup(Application &app) { (void)app; }
