/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** PluginLoader
*/

#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class IPlugin;

// Opens the plugin libraries (.so on Linux, .dll on Windows) and keeps them loaded until it is
// destroyed. It must outlive the Application and anything a plugin created (systems, components,
// thrown errors), because their code lives in the libraries
class PluginLoader
{
  public:
    // An empty directory means <directory of the executable>/plugins
    explicit PluginLoader(std::filesystem::path directory = {});
    ~PluginLoader();
    PluginLoader(PluginLoader const &) = delete;
    PluginLoader &operator=(PluginLoader const &) = delete;

    // Opens <directory>/<name>.so (or .dll) and calls its createPlugin entry point
    std::shared_ptr<IPlugin> load(std::string const &name);

  private:
    std::filesystem::path _directory;
    std::vector<void *> _libraries;
};
