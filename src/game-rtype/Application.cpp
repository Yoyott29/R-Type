/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#include "Application.hpp"
#include "AppAlive.hpp"
#include "Client.hpp"
#include "Physics.hpp"
#include "Position.hpp"
#include "RayLibRenderer.hpp"
#include "Sprite.hpp"
#include "Time.hpp"
#include "Velocity.hpp"
#include <algorithm>
#include <chrono>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define WINDOW_TITLE "R-Type"
#define MAX_FRAME_TIME 0.25f

Application::Application()
{
    // setup basic components and resources
    _scene.registerComponent<Position>();
    _scene.addResource<AppAlive>();

    // setup plugins
    addPlugin(std::make_shared<RayLibRenderer>());
    addPlugin(std::make_shared<Physics>());
    addPlugin(std::make_shared<Client>());

    // setup entities
    Entity playerOne = _scene.createEntity();
    _scene.addComponent(playerOne, Position{0.0f, 200.0f});
    _scene.addComponent(playerOne, Velocity{0.0f, 0.0f});
    _scene.addComponent(playerOne, Sprite{50.0f, 35.0f, Color{230, 41, 55, 255}});

    Entity decoration = _scene.createEntity();
    _scene.addComponent(decoration, Position{400.0f, 100.0f});
    _scene.addComponent(decoration, Sprite{64.0f, 16.0f, Color{0, 121, 241, 255}});
    _scene.addComponent(decoration, Velocity{100.0f, 20.0f});
}

void Application::addPlugin(std::shared_ptr<IPlugin> plugin)
{
    _plugins.push_back(plugin);
    plugin->build(*this);
}

void Application::run()
{
    using Clock = std::chrono::steady_clock;
    Clock::time_point previous = Clock::now();
    float accumulator = 0.0f;

    while (_scene.getResource<AppAlive>().alive) {
        Clock::time_point now = Clock::now();
        float frameTime =
            std::min(std::chrono::duration<float>(now - previous).count(), MAX_FRAME_TIME);
        previous = now;
        accumulator += frameTime;

        _scene.runSchedule(PrepUpdate);
        _scene.runSchedule(StateTransition);
        while (accumulator >= FIXED_DELTA_TIME) {
            _scene.runSchedule(FixedUpdate);
            accumulator -= FIXED_DELTA_TIME;
        }
        _scene.runSchedule(Update);
        _scene.runSchedule(PostUpdate);
    }

    for (auto &plugin : _plugins)
        plugin->cleanup(*this);
}
