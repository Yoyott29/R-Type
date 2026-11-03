/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics
*/

#include "Physics.hpp"
#include "Collider2D.hpp"
#include "Collider2DSystem.hpp"
#include "MovementSystem.hpp"
#include "PluginApi.hpp"
#include "Position.hpp"
#include "Velocity.hpp"
#include "ComponentJson.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>

std::uint32_t Physics::maskValue(const nlohmann::json &component)
{
    std::string text = ComponentJson::value(component, "TypeCollisions", "");
    if (text.empty())
        text = ComponentJson::value(component, "Mask", "All layers");
    if (text == "All" || text == "All layers")
        return std::numeric_limits<std::uint32_t>::max();
    if (text == "None")
        return 0;
    try {
        std::size_t parsed = 0;
        const unsigned long long result = std::stoull(text, &parsed, 0);
        if (parsed != text.size() || result > std::numeric_limits<std::uint32_t>::max())
            throw std::invalid_argument("invalid collision mask");
        return static_cast<std::uint32_t>(result);
    } catch (const std::exception &) {
        throw std::invalid_argument("Invalid collision mask: " + text);
    }
}

void Physics::registerComponents(Scene &scene)
{
    // register the Velocity and Collider2D components, with how to read them from a .gringo file
    scene.registerComponent<Velocity>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity, Velocity{ComponentJson::floatValue(data, "Dx", 0.0f),
                                             ComponentJson::floatValue(data, "Dy", 0.0f)});
    });
    scene.registerComponent<Collider2D>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            Collider2D collider;
            if (ComponentJson::value(data, "Shape", "Box") == "Circle")
                collider.shape = CircleShape{ComponentJson::floatValue(data, "Radius", 16.0f)};
            else
                collider.shape = BoxShape{ComponentJson::floatValue(data, "Width", 32.0f),
                                          ComponentJson::floatValue(data, "Height", 32.0f)};
            collider.layer = static_cast<std::uint32_t>(ComponentJson::intValue(data, "Layer", 1));
            collider.mask = maskValue(data);
            target.addComponent(entity, collider);
        });
}

void Physics::build(Application &app)
{
    // setup the MovementSystem and its component requirements
    auto movementSystem = app._scene.registerSystem<MovementSystem>(FixedUpdate);
    ComponentSet movementRequirements;
    movementRequirements.set(app._scene.getComponentType<Position>());
    movementRequirements.set(app._scene.getComponentType<Velocity>());
    app._scene.setSystemComponentSet<MovementSystem>(movementRequirements);

    // setup the Collider2DSystem, run after the movement
    app._scene.registerSystem<Collider2DSystem>(FixedUpdate);
    ComponentSet colliderRequirements;
    colliderRequirements.set(app._scene.getComponentType<Position>());
    colliderRequirements.set(app._scene.getComponentType<Collider2D>());
    app._scene.setSystemComponentSet<Collider2DSystem>(colliderRequirements);
}

void Physics::cleanup(Application &app)
{
    (void)app;
    // Cleanup logic for the Physics plugin
}

PLUGIN_EXPORT IPlugin *createPlugin() { return new Physics(); }
