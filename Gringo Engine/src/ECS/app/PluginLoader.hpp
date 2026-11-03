/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** PluginLoader
*/

#pragma once
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

class IPlugin;
class IRenderer;

// Opens the plugin and renderer libraries (.so on Linux, .dll on Windows). They are never closed:
// anything a plugin created (systems, components, thrown errors) runs its code from them until
// the process ends.
// A library needs no special function: its class derives from Plugin<T> (or Renderer<T>), which
// registers how to create it while the library opens. The loader then finds that registration by
// the library's file
class PluginLoader
{
  public:
    using PluginFactory = std::shared_ptr<IPlugin> (*)();
    using RendererFactory = std::shared_ptr<IRenderer> (*)();

    // folder: "plugins" or "renderers". An empty directory means the shared Gringo Engine/<folder>
    explicit PluginLoader(std::string const &folder = "plugins",
                          std::filesystem::path directory = {});

    // Opens <directory>/<name>.so (or .dll) and creates the plugin it contains
    std::shared_ptr<IPlugin> load(std::string const &name) const;
    // Same with a renderer library
    std::shared_ptr<IRenderer> loadRenderer(std::string const &name) const;

    // Lists the names of the libraries in the directory, without the .so or .dll extension
    std::vector<std::string> listNames() const;
    bool isAvailable(std::string const &name) const;

    // Folder of the running executable (the game, the editor...)
    static std::filesystem::path executableDirectory();

    // Called by Plugin<T> and Renderer<T> while their library opens. Always returns true
    static bool registerPlugin(PluginFactory create);
    static bool registerRenderer(RendererFactory create);

  private:
    // Opens the library, throws PluginLoadError if it cannot. Returns its path, as registered
    std::filesystem::path open(std::string const &name) const;

    // The library containing a registered create function, keyed by its file
    static std::map<std::filesystem::path, PluginFactory> &pluginFactories();
    static std::map<std::filesystem::path, RendererFactory> &rendererFactories();

    std::filesystem::path _directory;
};
