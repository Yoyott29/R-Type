/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Application
*/

#pragma once
#include "BuildVariant.hpp"
#include "ComponentDescriptor.hpp"
#include "IRenderer.hpp"
#include "PluginLoader.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "TickTimeManager.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class IPlugin;

class Application
{
  public:
    Application();
    // Keeps the arguments (without the program name) in the CommandLine resource
    explicit Application(std::vector<std::string> arguments);
    // Throws if a plugin needs a renderer and none is set, then runs until AppAlive is false or
    // the window is closed. FixedUpdate runs on the game clock (GameClock); without a renderer
    // the loop sleeps until the next tick
    void run();
    void addPlugin(const std::shared_ptr<IPlugin> &plugin);
    // Loads a plugin from the shared Gringo Engine/plugins directory.
    void addPlugin(std::string const &name);
    // Loads a renderer from Gringo Engine/renderers, opens its window and adds it to the scene.
    // Call it before adding the plugins that use it
    void setRenderer(std::string const &name, std::string const &windowTitle);
    bool hasRenderer() const { return static_cast<bool>(_renderer); }

    Scene _scene;

  private:
    void addPlugin(const std::shared_ptr<IPlugin> &plugin, std::string const &name);
    void checkRendererRequirements() const;
    // Opens the frame, runs Render and Overlay, shows the frame
    void renderFrame();

    PluginLoader _loader;
    PluginLoader _rendererLoader{"renderers"};
    std::vector<std::shared_ptr<IPlugin>> _plugins;
    std::vector<std::string> _pluginNames;
    std::shared_ptr<IRenderer> _renderer;
    TickTimeManager _clock;
};

class IPlugin
{
  public:
    virtual ~IPlugin() = default;
    virtual void registerComponents(Scene &scene) { (void)scene; }
    virtual void build(Application &app) = 0;
    virtual void cleanup(Application &app) = 0;

    // True if the plugin calls the renderer (IRenderer): it cannot run in a project without one
    virtual bool requiresRenderer() const { return false; }
    // Project settings of the plugin, edited in the editor's Settings panel and read in build()
    // from the ProjectSettings resource
    virtual std::vector<PropertyDescriptor> settings() const { return {}; }
    // The executables this plugin adds to the project's build, besides the game itself
    virtual std::vector<BuildVariant> buildVariants() const { return {}; }
};

// Base class of every plugin: class Physics : public Plugin<Physics>. When the plugin's library
// is opened, it registers how to create the plugin, so the engine can load it by library name
template <typename Derived> class Plugin : public IPlugin
{
  public:
    // Uses _registered, so it exists (and runs) as soon as the plugin class is compiled
    ~Plugin() override { (void)_registered; }

  private:
    static std::shared_ptr<IPlugin> create() { return std::make_shared<Derived>(); }
    static inline const bool _registered = PluginLoader::registerPlugin(&Plugin::create);
};
