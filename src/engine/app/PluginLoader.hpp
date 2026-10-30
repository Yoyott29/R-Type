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

class IPlugin;

// Opens the plugin libraries (.so on Linux, .dll on Windows). They are never closed: anything a
// plugin created (systems, components, thrown errors) runs its code from them until the process
// ends
class PluginLoader
{
  public:
    // An empty directory means <directory of the executable>/plugins
    explicit PluginLoader(std::filesystem::path directory = {});

    // Opens <directory>/<name>.so (or .dll) and calls its createPlugin entry point
    std::shared_ptr<IPlugin> load(std::string const &name) const;

  private:
    std::filesystem::path _directory;
};
