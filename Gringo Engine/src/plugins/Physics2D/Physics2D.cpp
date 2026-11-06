/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics2D
*/

#include "Physics2D.hpp"
#include "Collider2D.hpp"
#include "Collider2DSystem.hpp"
#include "Gravity2D.hpp"
#include "GravitySystem.hpp"
#include "MovementSystem.hpp"
#include "Transform.hpp"
#include "ProjectSettings.hpp"
#include "RigidBody2D.hpp"
#include "Velocity.hpp"
#include "ComponentJson.hpp"
#include <cstdint>
#include <limits>
#include <stdexcept>

std::uint32_t Physics2D::maskValue(const nlohmann::json &component)
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

void Physics2D::registerComponents(Scene &scene)
{
    // register the Velocity, Collider2D and RigidBody2D components, with how to read them from a .gringo file
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
            collider.isTrigger = ComponentJson::boolValue(data, "IsTrigger", false);
            target.addComponent(entity, collider);
        });
    scene.registerComponent<RigidBody2D>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            RigidBody2D body;
            body.mass = ComponentJson::floatValue(data, "Mass", 1.0f);
            if (body.mass <= 0.0f)
                throw std::invalid_argument("RigidBody2D Mass must be positive");
            body.isStatic = ComponentJson::boolValue(data, "IsStatic", false);
            body.gravityScale = ComponentJson::floatValue(data, "GravityScale", 1.0f);
            target.addComponent(entity, body);
        });
}

std::vector<PropertyDescriptor> Physics2D::settings() const
{
    return {
        {"GravityX", valueType::Float, "0"},
        {"GravityY", valueType::Float, "980"},
    };
}

float Physics2D::floatSetting(Scene &scene, const std::string &key) const
{
    const std::string text = scene.getResource<ProjectSettings>().get("Physics2D", key);
    try {
        std::size_t parsed = 0;
        const float value = std::stof(text, &parsed);
        if (parsed != text.size())
            throw std::invalid_argument("trailing characters");
        return value;
    } catch (const std::exception &) {
        throw std::invalid_argument("Invalid Physics2D setting " + key + ": " + text);
    }
}

void Physics2D::build(Application &app)
{
    app._scene.addResource<Gravity2D>(Gravity2D{floatSetting(app._scene, "GravityX"),
                                                floatSetting(app._scene, "GravityY")});

    // setup the GravitySystem, run before the movement so it uses the new velocity
    app._scene.registerSystem<GravitySystem>(FixedUpdate);
    ComponentSet gravityRequirements;
    gravityRequirements.set(app._scene.getComponentType<RigidBody2D>());
    gravityRequirements.set(app._scene.getComponentType<Velocity>());
    app._scene.setSystemComponentSet<GravitySystem>(gravityRequirements);

    // setup the MovementSystem and its component requirements
    auto movementSystem = app._scene.registerSystem<MovementSystem>(FixedUpdate);
    ComponentSet movementRequirements;
    movementRequirements.set(app._scene.getComponentType<Transform>());
    movementRequirements.set(app._scene.getComponentType<Velocity>());
    app._scene.setSystemComponentSet<MovementSystem>(movementRequirements);

    // setup the Collider2DSystem, run after the movement
    app._scene.registerSystem<Collider2DSystem>(FixedUpdate);
    ComponentSet colliderRequirements;
    colliderRequirements.set(app._scene.getComponentType<Transform>());
    colliderRequirements.set(app._scene.getComponentType<Collider2D>());
    app._scene.setSystemComponentSet<Collider2DSystem>(colliderRequirements);
}

void Physics2D::cleanup(Application &app)
{
    (void)app;
    // Cleanup logic for the Physics2D plugin
}
