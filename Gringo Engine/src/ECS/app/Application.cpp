/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#include "Application.hpp"
#include "AppAlive.hpp"
#include "CommandLine.hpp"
#include "ProjectSettings.hpp"
#include "EngineError.hpp"
#include "Position.hpp"
#include "Time.hpp"
#include <algorithm>
#include <chrono>
#include <functional>

#define WORLD_WIDTH 1920
#define WORLD_HEIGHT 1080
#define MAX_FRAME_TIME 0.25f

static constexpr ColorRGBA BACKGROUND_COLOR{0, 0, 0, 255};

Application::Application()
{
    // setup basic components and resources
    _scene.registerComponent<Position>();
    _scene.addResource<AppAlive>();
    _scene.addResource<CommandLine>();
    _scene.addResource<ProjectSettings>();
}

Application::Application(int argc, char **argv) : Application()
{
    for (int index = 1; index < argc; ++index)
        _scene.getResource<CommandLine>().arguments.emplace_back(argv[index]);
}

void Application::addPlugin(const std::shared_ptr<IPlugin> &plugin) { addPlugin(plugin, ""); }

void Application::addPlugin(std::string const &name) { addPlugin(_loader.load(name), name); }

void Application::addPlugin(const std::shared_ptr<IPlugin> &plugin, std::string const &name)
{
    _plugins.push_back(plugin);
    _pluginNames.push_back(name);
    if (!name.empty())
        _scene.getResource<ProjectSettings>().addDefaults(name, plugin->settings());
    plugin->registerComponents(_scene);
    plugin->build(*this);
}

void Application::setRenderer(std::string const &name, std::string const &windowTitle)
{
    _renderer = _rendererLoader.loadRenderer(name);
    _renderer->openWindow(WORLD_WIDTH, WORLD_HEIGHT, windowTitle);
    _scene.addResource<std::reference_wrapper<IRenderer>>(*_renderer);
}

void Application::checkRendererRequirements() const
{
    if (_renderer)
        return;
    for (std::size_t index = 0; index < _plugins.size(); ++index)
        if (_plugins[index]->requiresRenderer())
            throw EngineError("Plugin " +
                              (_pluginNames[index].empty() ? "<unnamed>" : _pluginNames[index]) +
                              " needs a renderer: call setRenderer before run");
}

void Application::renderFrame()
{
    _renderer->beginFrame(BACKGROUND_COLOR);
    _scene.runSchedule(Render);
    _scene.runSchedule(Overlay);
    _renderer->endFrame();
    if (!_renderer->isWindowOpen())
        _scene.getResource<AppAlive>().alive = false;
}

void Application::run()
{
    checkRendererRequirements();
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
        if (_renderer)
            renderFrame();
    }

    for (auto &plugin : _plugins)
        plugin->cleanup(*this);
    if (_renderer)
        _renderer->closeWindow();
}
