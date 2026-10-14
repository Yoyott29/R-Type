/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#include "Application.hpp"
#include "PlayerControlled.hpp"
#include "Position.hpp"
#include "RaylibRenderer.hpp"
#include "Sprite.hpp"
#include "Time.hpp"
#include "Velocity.hpp"

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "R-Type"
#define MAX_FRAME_TIME 0.25f

Application::Application() : _renderer(std::make_unique<RaylibRenderer>())
{
    _renderer->init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    setupComponents();
    setupSystems();
    setupEntities();
    _game = Game();
}

void Application::setupComponents()
{
    _scene.registerComponent<Position>();
    _scene.registerComponent<Velocity>();
    _scene.registerComponent<Sprite>();
    _scene.registerComponent<PlayerControlled>();
    _scene.registerComponent<Collider2D>();
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
    renderRequirements.set(_scene.getComponentType<Collider2D>());
    _scene.setSystemComponentSet<RenderSystem>(renderRequirements);

    _Collider2DSystem = _scene.registerSystem<Collider2DSystem>();
    ComponentSet colliderRequirements;
    colliderRequirements.set(_scene.getComponentType<Position>());
    colliderRequirements.set(_scene.getComponentType<Collider2D>());
    _scene.setSystemComponentSet<Collider2DSystem>(colliderRequirements);
}

void Application::setupEntities()
{
    Entity playerOne = _scene.createEntity();
    _scene.addComponent(playerOne, Position{0.0f, 200.0f});
    _scene.addComponent(playerOne, Velocity{0.0f, 0.0f});
    _scene.addComponent(playerOne, Sprite{BoxShape{50.0f, 35.0f}, ColorRGBA{0, 228, 48, 255}});
    _scene.addComponent(playerOne, PlayerControlled{});
    _scene.addComponent(
        playerOne, Collider2D{BoxShape{50.0f, 35.0f},
                              [&](Entity player, Entity enemy) { _scene.queueDestroy(player); }});
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
            _Collider2DSystem->update(_scene);
            _scene.flushDestroyQueue();
            _game.runGameLoop(_scene, *_renderer);
            accumulator -= FIXED_DELTA_TIME;
        }

        _renderer->beginFrame();
        _renderer->clear(ColorRGBA{0, 0, 0, 255});
        _renderSystem->update(_scene, *_renderer);
        _renderer->endFrame();
    }
}
