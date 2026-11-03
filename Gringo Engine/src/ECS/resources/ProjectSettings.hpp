/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ProjectSettings
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

// Scene resource: the settings of each plugin for this project, saved in the .gringo file as
// "pluginSettings": {"Network": {"Port": "4242"}}. A plugin declares its settings with
// IPlugin::settings() and reads them in build()
class ProjectSettings
{
  public:
    ProjectSettings() = default;
    explicit ProjectSettings(std::map<std::string, std::map<std::string, std::string>> values)
        : _values(std::move(values))
    {
    }

    static ProjectSettings fromJson(const nlohmann::json &json)
    {
        std::map<std::string, std::map<std::string, std::string>> values;
        for (const auto &[plugin, settings] : json.items())
            for (const auto &[key, value] : settings.items())
                values[plugin][key] = value.get<std::string>();
        return ProjectSettings(std::move(values));
    }

    // The setting's value, "" if unknown. Defaults are filled in when the plugin is added
    std::string get(const std::string &plugin, const std::string &key) const
    {
        auto settings = _values.find(plugin);
        if (settings == _values.end())
            return "";
        auto value = settings->second.find(key);
        return value == settings->second.end() ? "" : value->second;
    }

    // Called by the application when it adds a plugin: fills what the project does not set
    void addDefaults(const std::string &plugin, const std::vector<PropertyDescriptor> &settings)
    {
        for (const PropertyDescriptor &setting : settings)
            _values[plugin].try_emplace(setting.name, setting.defaultValue);
    }

  private:
    std::map<std::string, std::map<std::string, std::string>> _values;
};
