/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** ComponentCatalog
*/

#ifndef COMPONENTCATALOG_HPP_
#define COMPONENTCATALOG_HPP_

#include "ComponentDescriptor.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

class ComponentCatalog
{
  public:
    ComponentCatalog();

    const std::vector<ComponentDescriptor> &getDescriptors() const { return _descriptors; }

    // Settings declared by each plugin (IPlugin::settings), shown in the editor's Settings panel
    const std::map<std::string, std::vector<PropertyDescriptor>> &getPluginSettings() const
    {
        return _pluginSettings;
    }

  private:
    std::vector<ComponentDescriptor> _descriptors;
    std::map<std::string, std::vector<PropertyDescriptor>> _pluginSettings;

    // Kept alive until the end of the program, the type is erased to hide IPlugin from this header
    std::vector<std::shared_ptr<void>> _loadedPlugins;
};

#endif /* !COMPONENTCATALOG_HPP_ */
