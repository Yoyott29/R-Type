/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#pragma once
#include "PluginLoader.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class IPlugin;

class Application
{
  public:
    Application();
    void run();
    void addPlugin(const std::shared_ptr<IPlugin> &plugin);
    // Loads plugins/<name>.so (.dll on Windows) next to the executable and adds it
    void addPlugin(std::string const &name);

    Scene _scene;

  private:
    PluginLoader _loader;
    std::vector<std::shared_ptr<IPlugin>> _plugins;
};

class IPlugin
{
  public:
    virtual ~IPlugin() = default;
    virtual void build(Application &app) = 0;
    virtual void cleanup(Application &app) = 0;
};
