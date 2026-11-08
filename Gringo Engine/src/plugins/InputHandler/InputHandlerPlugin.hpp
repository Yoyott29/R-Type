/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** InputHandlerPlugin
*/

#pragma once
#include "Application.hpp"

// Keyboard state for scripts and systems: the KeyboardInput resource, read from the renderer
class InputHandlerPlugin : public Plugin<InputHandlerPlugin>
{
  public:
    void build(Application &app) override;
    void cleanup(Application &app) override;
    bool requiresRenderer() const override { return true; }
};
