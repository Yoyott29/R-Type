#include "ProjectComponentManager.hpp"
#ifdef GRINGO_USE_PHYSICS
#include "Collider2D.hpp"
#include "Velocity.hpp"
#endif
#include "Position.hpp"
#ifdef GRINGO_USE_RENDERER
#include "Sprite.hpp"
#include "SoundEffect.hpp"
#endif
#include "Velocity.hpp"
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>

namespace {
std::string value(const nlohmann::json &component, const std::string &name,
                  const std::string &fallback)
{
    for (const auto &property : component.value("properties", nlohmann::json::array()))
        if (property.value("name", "") == name)
            return property.value("value", fallback);
    return fallback;
}

float floatValue(const nlohmann::json &component, const std::string &name, float fallback)
{
    return std::stof(value(component, name, std::to_string(fallback)));
}

int intValue(const nlohmann::json &component, const std::string &name, int fallback)
{
    return std::stoi(value(component, name, std::to_string(fallback)));
}

bool boolValue(const nlohmann::json &component, const std::string &name, bool fallback)
{
    const std::string result = value(component, name, fallback ? "True" : "False");
    return result == "True" || result == "true" || result == "1";
}

ColorRGBA colorValue(const nlohmann::json &component, const std::string &name,
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
}

ProjectComponentManager::ProjectComponentManager(Scene &scene,
                                                 std::filesystem::path projectFolder)
    : _projectFolder(std::move(projectFolder))
{
    scene.registerComponent<Position>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity, Position{floatValue(data, "PositionX", 0.0f),
                                             floatValue(data, "PositionY", 0.0f)});
    });
#ifdef GRINGO_USE_PHYSICS
    scene.registerComponent<Velocity>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity,
                            Velocity{floatValue(data, "Dx", 0.0f), floatValue(data, "Dy", 0.0f)});
    });
    scene.registerComponent<Collider2D>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            Collider2D collider;
            if (value(data, "Shape", "Box") == "Circle")
                collider.shape = CircleShape{floatValue(data, "Radius", 16.0f)};
            else
                collider.shape = BoxShape{floatValue(data, "Width", 32.0f),
                                          floatValue(data, "Height", 32.0f)};
            collider.layer = static_cast<std::uint32_t>(intValue(data, "Layer", 1));
            collider.mask =
                static_cast<std::uint32_t>(std::stoull(value(data, "Mask", "4294967295")));
            target.addComponent(entity, collider);
        });
#endif
#ifdef GRINGO_USE_RENDERER
    scene.registerComponent<Sprite>([this](Scene &target, Entity entity,
                                           const nlohmann::json &data) {
        Sprite sprite;
        if (value(data, "Shape", "Box") == "Circle")
            sprite.shape = CircleShape{floatValue(data, "Radius", 16.0f)};
        else
            sprite.shape = BoxShape{floatValue(data, "Width", 32.0f),
                                    floatValue(data, "Height", 32.0f)};
        sprite.drawOrder = intValue(data, "DrawOrder", 0);
        const std::string texture = value(data, "TexturePath", "");
        sprite.texturePath = texture == "default.png"
                                 ? ""
                                 : assetPath(texture);
        sprite.color = colorValue(data, "Color", {255, 255, 255, 255});
        target.addComponent(entity, sprite);
    });
    scene.registerComponent<SoundEffect>([this](Scene &target, Entity entity,
                                                const nlohmann::json &data) {
        target.addComponent(entity,
                            SoundEffect{assetPath(value(data, "Path", "sound.wav")),
                                        boolValue(data, "Play", false)});
    });
#endif
}

void ProjectComponentManager::addToScene(Scene &scene, Entity entity,
                                         const nlohmann::json &component) const
{
    if (component.value("name", "") == "Script")
        return;
    scene.addComponentFromJson(entity, component);
}

void ProjectComponentManager::addToPrefab(Prefab &prefab, Scene &scene,
                                          const nlohmann::json &component) const
{
    if (component.value("name", "") == "Script")
        return;
    prefab.addComponent(scene.componentInitializer(component));
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
