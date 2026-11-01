#include "ProjectComponentManager.hpp"
#include "Collider2D.hpp"
#include "Position.hpp"
#include "SoundEffect.hpp"
#include "Sprite.hpp"
#include "Velocity.hpp"
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <unordered_map>

std::string ProjectComponentManager::propertyValue(const nlohmann::json &component,
                                                   const std::string &name,
                                                   const std::string &fallback)
{
    for (const auto &property : component.value("properties", nlohmann::json::array())) {
        if (property.value("name", "") == name)
            return property.value("value", fallback);
    }
    return fallback;
}

float ProjectComponentManager::floatProperty(const nlohmann::json &component,
                                             const std::string &name, float fallback)
{
    return std::stof(propertyValue(component, name, std::to_string(fallback)));
}

int ProjectComponentManager::intProperty(const nlohmann::json &component, const std::string &name,
                                         int fallback)
{
    return std::stoi(propertyValue(component, name, std::to_string(fallback)));
}

bool ProjectComponentManager::boolProperty(const nlohmann::json &component, const std::string &name,
                                           bool fallback)
{
    const std::string value = propertyValue(component, name, fallback ? "True" : "False");
    return value == "True" || value == "true" || value == "1";
}

ColorRGBA ProjectComponentManager::colorProperty(const nlohmann::json &component,
                                                 const std::string &name, ColorRGBA fallback)
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

    const auto color = colors.find(propertyValue(component, name, ""));
    return color != colors.end() ? color->second : fallback;
}

std::string ProjectComponentManager::assetPath(const std::string &path) const
{
    if (path.empty() || std::filesystem::path(path).is_absolute())
        return path;

    std::filesystem::path relativePath = path;
    if (relativePath.begin() != relativePath.end() &&
        (*relativePath.begin() == "assets" || *relativePath.begin() == "Assets"))
        relativePath = relativePath.lexically_relative(*relativePath.begin());

    return (_projectFolder / "assets" / relativePath).string();
}

void ProjectComponentManager::addToScene(Scene &scene, Entity entity,
                                         const nlohmann::json &component) const
{
    const std::string name = component.value("name", "");

    if (name == "Position") {
        scene.addComponent(entity, Position{floatProperty(component, "PositionX", 0.0f),
                                            floatProperty(component, "PositionY", 0.0f)});
    } else if (name == "Velocity") {
        scene.addComponent(entity, Velocity{floatProperty(component, "Dx", 0.0f),
                                            floatProperty(component, "Dy", 0.0f)});
    } else if (name == "Sprite") {
        Sprite sprite;
        if (propertyValue(component, "Shape", "Box") == "Circle")
            sprite.shape = CircleShape{floatProperty(component, "Radius", 16.0f)};
        else
            sprite.shape = BoxShape{floatProperty(component, "Width", 32.0f),
                                    floatProperty(component, "Height", 32.0f)};
        sprite.drawOrder = intProperty(component, "DrawOrder", 0);
        const std::string texturePath = propertyValue(component, "TexturePath", "");
        if (texturePath == "default.png")
            sprite.texturePath.clear();
        else
            sprite.texturePath = assetPath(texturePath);
        sprite.color = colorProperty(component, "Color", {255, 255, 255, 255});
        scene.addComponent(entity, sprite);
    } else if (name == "Collider2D") {
        Collider2D collider;
        if (propertyValue(component, "Shape", "Box") == "Circle")
            collider.shape = CircleShape{floatProperty(component, "Radius", 16.0f)};
        else
            collider.shape = BoxShape{floatProperty(component, "Width", 32.0f),
                                      floatProperty(component, "Height", 32.0f)};
        collider.layer = static_cast<std::uint32_t>(intProperty(component, "Layer", 1));
        collider.mask =
            static_cast<std::uint32_t>(std::stoull(propertyValue(component, "Mask", "4294967295")));
        scene.addComponent(entity, collider);
    } else if (name == "SoundEffect") {
        scene.addComponent(entity,
                           SoundEffect{assetPath(propertyValue(component, "Path", "sound.wav")),
                                       boolProperty(component, "Play", false)});
    } else if (name == "Script") {
        std::cerr << "Script path resolves from " << (_projectFolder / "scripts").string()
                  << ", but runtime script loading is not implemented: "
                  << propertyValue(component, "Path", "script.hpp") << '\n';
    } else {
        std::cerr << "Unsupported component in project: " << name << '\n';
    }
}
