/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#pragma once
#include "Game.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include <memory>
#include <vector>

class IPlugin;

class Application
{
  public:
    Application();
    void run();
    void addPlugin(std::shared_ptr<IPlugin> plugin);

    Scene _scene;
  private:
    void setupPrefabs();

    std::vector<std::shared_ptr<IPlugin>> _plugins;
    Game _game;
    PrefabsManager _prefabs;
};

class IPlugin {
    public:
        virtual ~IPlugin() = default;
        virtual void build(Application &app) = 0;
        virtual void cleanup(Application &app) = 0;
};
