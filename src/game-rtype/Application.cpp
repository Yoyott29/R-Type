/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#include "Application.hpp"
#include "RaylibRenderer.hpp"
#include "Time.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "Sprite.hpp"
#include "PlayerControlled.hpp"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "R-Type"
#define MAX_FRAME_TIME 0.25f

Application::Application() : _renderer(std::make_unique<RaylibRenderer>())
{
    _renderer->init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    setupComponents();
    setupSystems();
    setupEntities();
}

void Application::setupComponents()
{
    _scene.registerComponent<Position>();
    _scene.registerComponent<Velocity>();
    _scene.registerComponent<Sprite>();
    _scene.registerComponent<PlayerControlled>();
}

void Application::setupSystems()
{
    _inputSystem = _scene.registerSystem<InputSystem>();
    ComponentSet inputRequirements;
    inputRequirements.set(_scene.getComponentType<PlayerControlled>());
    inputRequirements.set(_scene.getComponentType<Velocity>());
    _scene.setSystemComponentSet<InputSystem>(inputRequirements);

    _movementSystem = _scene.registerSystem<MovementSystem>();
    ComponentSet movementRequirements;
    movementRequirements.set(_scene.getComponentType<Position>());
    movementRequirements.set(_scene.getComponentType<Velocity>());
    _scene.setSystemComponentSet<MovementSystem>(movementRequirements);

    _renderSystem = _scene.registerSystem<RenderSystem>();
    ComponentSet renderRequirements;
    renderRequirements.set(_scene.getComponentType<Position>());
    renderRequirements.set(_scene.getComponentType<Sprite>());
    _scene.setSystemComponentSet<RenderSystem>(renderRequirements);
}

void Application::setupEntities()
{
    Entity playerOne = _scene.createEntity();
    _scene.addComponent(playerOne, Position{0.0f, 200.0f});
    _scene.addComponent(playerOne, Velocity{0.0f, 0.0f});
    _scene.addComponent(playerOne, Sprite{50.0f, 35.0f, ColorRGBA{230, 41, 55, 255}});
    _scene.addComponent(playerOne, PlayerControlled{});

    Entity decoration = _scene.createEntity();
    _scene.addComponent(decoration, Position{400.0f, 100.0f});
    _scene.addComponent(decoration, Sprite{64.0f, 16.0f, ColorRGBA{0, 121, 241, 255}});
    _scene.addComponent(decoration, Velocity{100.0f, 20.0f});
}

void Application::run()
{
    float accumulator = 0.0f;

    while (!_renderer->shouldClose()) {
        float frameTime = _renderer->getFrameTime();
        if (frameTime > MAX_FRAME_TIME)
            frameTime = MAX_FRAME_TIME;
        accumulator += frameTime;

        while (accumulator >= FIXED_DELTA_TIME) {
            _inputSystem->update(_scene);
            _movementSystem->update(_scene);
            accumulator -= FIXED_DELTA_TIME;
        }

        _renderer->beginFrame();
        _renderer->clear(ColorRGBA{0, 0, 0, 255});
        _renderSystem->update(_scene, *_renderer);
        _renderer->endFrame();
    }
}
