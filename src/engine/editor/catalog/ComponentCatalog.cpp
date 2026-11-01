/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** ComponentCatalog
*/

#include "ComponentCatalog.hpp"
#include "Application.hpp"
#include "PluginLoader.hpp"
#include <exception>
#include <iostream>

ComponentCatalog::ComponentCatalog()
{
    // The application registers the engine components (Position, Script) in its constructor
    Application application;
    PluginLoader pluginLoader;

    for (const std::string &pluginName : pluginLoader.listNames()) {
        try {
            std::shared_ptr<IPlugin> plugin = pluginLoader.load(pluginName);

            // Only the components are registered, build() would open windows and sockets
            plugin->registerComponents(application._scene);
            _loadedPlugins.push_back(plugin);
        } catch (const std::exception &error) {
            std::cerr << "Skipping plugin " << pluginName << ": " << error.what() << std::endl;
        }
    }

    // Copy the descriptors, the application is destroyed at the end of this constructor
    _descriptors = application._scene.getComponentDescriptors();
}
