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
#include "Script.hpp"
#include "ScriptSystem.hpp"
#include "Sprite.hpp"
#include "Time.hpp"
#include "Type.hpp"
#include "Velocity.hpp"
#include <algorithm>
#include <chrono>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "R-Type"
#define MAX_FRAME_TIME 0.25f

Application::Application() : _game(_prefabs)
{
    // setup basic components and resources
    _scene.registerComponent<Position>();
    _scene.registerComponent<Script>();
    _scene.registerComponent<Type>();
    _scene.addResource<AppAlive>();

    // setup plugins
    addPlugin(std::make_shared<RayLibRenderer>());
    addPlugin(std::make_shared<Physics>());
    addPlugin(std::make_shared<Client>());

    // setup gameplay systems, after the Physics systems in the fixed update
    _scene.registerSystem<ScriptSystem>(FixedUpdate);
    ComponentSet scriptRequirements;
    scriptRequirements.set(_scene.getComponentType<Script>());
    _scene.setSystemComponentSet<ScriptSystem>(scriptRequirements);

    setupPrefabs();

    // setup entities
    Entity playerOne = _scene.createEntity();
    _scene.addComponent(playerOne, Position{0.0f, 200.0f});
    _scene.addComponent(playerOne, Velocity{0.0f, 0.0f});
    _scene.addComponent(playerOne, Sprite{BoxShape{50.0f, 35.0f}, Color{0, 228, 48, 255}});
    _scene.addComponent(playerOne, Collider2D{BoxShape{50.0f, 35.0f}, [&](Entity player, Entity) {
                                                  _scene.queueDestroy(player);
                                              }});

    Entity spawner = _scene.createEntity();
    _scene.addComponent(spawner, Script{Timing::ON_UPDATE, [this](Scene &scene, Entity &) {
                                            _game.SpawnEnemy(scene);
                                        }});
}

void Application::setupPrefabs()
{

    _prefabs.createPrefab(
        "Enemy", Prefab()
                     .addComponent(Position{2000.0f, 0.0f})
                     .addComponent(Velocity{-150.0f, 0.0f})
                     .addComponent(Sprite{BoxShape{50.0f, 35.0f}, Color{228, 48, 0, 255}})
                     .addComponent(Collider2D{BoxShape{50.0f, 35.0f}, nullptr})
                     .addComponent(Script{Timing::AT_LAUNCH, [this](Scene &scene, Entity &enemy) {
                                              _game.InitializeEnemyPosition(scene, enemy);
                                          }}));
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
