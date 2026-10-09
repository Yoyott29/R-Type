/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#pragma once
#include "IRenderer.hpp"
#include "InputSystem.hpp"
#include "MovementSystem.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include <memory>

class Application
{
  public:
    Application();
    void run();

  private:
    void setupComponents();
    void setupEntities();
    void setupSystems();

    Scene _scene;
    std::unique_ptr<IRenderer> _renderer;
    std::shared_ptr<InputSystem> _inputSystem;
    std::shared_ptr<MovementSystem> _movementSystem;
    std::shared_ptr<RenderSystem> _renderSystem;
};
