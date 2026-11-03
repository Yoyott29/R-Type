/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ComponentJson
*/

#pragma once
#include "ColorRGBA.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

// Helpers for the deserializers plugins give to Scene::registerComponent<T>(deserializer). They
// read one property of a component as written in a .gringo file:
//   {"name": "Velocity", "properties": [{"name": "Dx", "value": "-500"}, ...]}
// The property names are the ones of the component's ComponentDescription
class ComponentJson
{
  public:
    ComponentJson() = delete;

    static std::string value(const nlohmann::json &component, const std::string &name,
                             const std::string &fallback)
    {
        for (const auto &property : component.value("properties", nlohmann::json::array()))
            if (property.value("name", "") == name)
                return property.value("value", fallback);
        return fallback;
    }

    static float floatValue(const nlohmann::json &component, const std::string &name,
                            float fallback)
    {
        return std::stof(value(component, name, std::to_string(fallback)));
    }

    static int intValue(const nlohmann::json &component, const std::string &name, int fallback)
    {
        return std::stoi(value(component, name, std::to_string(fallback)));
    }

    static bool boolValue(const nlohmann::json &component, const std::string &name, bool fallback)
    {
        const std::string result = value(component, name, fallback ? "True" : "False");
        return result == "True" || result == "true" || result == "1";
    }

    static ColorRGBA colorValue(const nlohmann::json &component, const std::string &name,
                                ColorRGBA fallback)
    {
        static const std::unordered_map<std::string, ColorRGBA> colors = {
            {"Red", {255, 0, 0, 255}},       {"Orange", {255, 165, 0, 255}},
            {"Yellow", {255, 255, 0, 255}},  {"Lime", {0, 255, 0, 255}},
            {"Green", {0, 128, 0, 255}},     {"Cyan", {0, 255, 255, 255}},
            {"Blue", {0, 0, 255, 255}},      {"Purple", {128, 0, 128, 255}},
            {"Magenta", {255, 0, 255, 255}}, {"Pink", {255, 192, 203, 255}},
            {"Brown", {139, 69, 19, 255}},   {"White", {255, 255, 255, 255}},
            {"Gray", {128, 128, 128, 255}},  {"Black", {0, 0, 0, 255}},
            {"Transparent", {0, 0, 0, 0}},
        };
        auto iterator = colors.find(value(component, name, ""));
        return iterator == colors.end() ? fallback : iterator->second;
    }
};
