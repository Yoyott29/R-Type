/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#pragma once
#include <memory>
#include "Scene.hpp"
#include "IRenderer.hpp"
#include "MovementSystem.hpp"
#include "RenderSystem.hpp"
#include "InputSystem.hpp"

class Application {
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
