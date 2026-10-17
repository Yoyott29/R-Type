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
    void addPlugin(const std::shared_ptr<IPlugin> &plugin);

    Scene _scene;

  private:
    std::vector<std::shared_ptr<IPlugin>> _plugins;
};

class IPlugin
{
  public:
    virtual ~IPlugin() = default;
    virtual void build(Application &app) = 0;
    virtual void cleanup(Application &app) = 0;
};
