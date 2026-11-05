/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#include "Application.hpp"
#include "AppAlive.hpp"
#include "ChildOf.hpp"
#include "HierarchySystem.hpp"
#include "CommandLine.hpp"
#include "ProjectSettings.hpp"
#include "EngineError.hpp"
#include "GameClock.hpp"
#include "EventBus.hpp"
#include "Position.hpp"
#include <thread>
#include <utility>
#include <functional>

#define WORLD_WIDTH 1920
#define WORLD_HEIGHT 1080
// After a stall, at most this many late ticks are run (0.25 s), the others are dropped
#define MAX_CATCH_UP_TICKS 15

static const ColorRGBA BACKGROUND_COLOR{0, 0, 0, 255};

Application::Application()
{
    // setup basic components and resources
    _scene.registerComponent<Position>();
    _scene.registerComponent<ChildOf>();
    _scene.registerSystem<HierarchySystem>(Update);
    _scene.addResource<AppAlive>();
    _scene.addResource<CommandLine>();
    _scene.addResource<ProjectSettings>();
    _scene.addResource<GameClock>();
    _scene.addResource<EventBus>();
}

Application::Application(std::vector<std::string> arguments) : Application()
{
    _scene.getResource<CommandLine>().arguments = std::move(arguments);
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
    _clock.start();

    while (_scene.getResource<AppAlive>().alive) {
        _scene.runSchedule(PrepUpdate);
        _scene.runSchedule(StateTransition);
        for (int ticks = 0; _clock.isTickDue(); ++ticks) {
            if (ticks == MAX_CATCH_UP_TICKS) {
                _clock.resync();
                break;
            }
            _scene.runSchedule(FixedUpdate);
            _scene.flushDestroyQueue();
            _clock.advance();
            _scene.getResource<GameClock>().tick = _clock.getCurrentTick();
        }
        _scene.runSchedule(Update);
        _scene.runSchedule(PostUpdate);
        if (_renderer)
            renderFrame();
        else
            std::this_thread::sleep_until(_clock.getNextTickTime());
    }

    for (auto &plugin : _plugins)
        plugin->cleanup(*this);
    if (_renderer)
        _renderer->closeWindow();
}
