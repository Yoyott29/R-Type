/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#include "Application.hpp"
#include "AppAlive.hpp"
#include "Position.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include <algorithm>
#include <chrono>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "R-Type"
#define MAX_FRAME_TIME 0.25f

Application::Application()
{
    // setup basic components and resources
    _scene.registerComponent<Position>();
    _scene.registerComponent<Script>();
    _scene.addResource<AppAlive>();
}

void Application::addPlugin(const std::shared_ptr<IPlugin> &plugin)
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
            _scene.flushDestroyQueue();
            accumulator -= FIXED_DELTA_TIME;
        }
        _scene.runSchedule(Update);
        _scene.runSchedule(PostUpdate);
    }

    for (auto &plugin : _plugins)
        plugin->cleanup(*this);
}
