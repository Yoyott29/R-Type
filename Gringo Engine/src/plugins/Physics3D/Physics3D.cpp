/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics3D
*/

#include "Physics3D.hpp"
#include "ArcWall3D.hpp"
#include "ArcWall3DSystem.hpp"
#include "Collider3D.hpp"
#include "Collider3DSystem.hpp"
#include "ComponentJson.hpp"
#include "Gravity3D.hpp"
#include "Gravity3DSystem.hpp"
#include "Movement3DSystem.hpp"
#include "Transform.hpp"
#include "ProjectSettings.hpp"
#include "RigidBody3D.hpp"
#include "Velocity3D.hpp"
#include <limits>
#include <stdexcept>
#include <utility>

std::uint32_t Physics3D::maskValue(const nlohmann::json &component)
{
    std::string text = ComponentJson::value(component, "TypeCollisions", "All layers");
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

void Physics3D::registerComponents(Scene &scene)
{
    scene.registerComponent<Velocity3D>([](Scene &target, Entity entity, const nlohmann::json &data) {
        target.addComponent(entity, Velocity3D{ComponentJson::floatValue(data, "Dx", 0.0f),
                                               ComponentJson::floatValue(data, "Dy", 0.0f),
                                               ComponentJson::floatValue(data, "Dz", 0.0f)});
    });
    scene.registerComponent<RigidBody3D>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            RigidBody3D body;
            body.mass = ComponentJson::floatValue(data, "Mass", 1.0f);
            if (body.mass <= 0.0f)
                throw std::invalid_argument("RigidBody3D Mass must be positive");
            body.isStatic = ComponentJson::boolValue(data, "IsStatic", false);
            body.gravityScale = ComponentJson::floatValue(data, "GravityScale", 1.0f);
            body.stepHeight = ComponentJson::floatValue(data, "StepHeight", 0.0f);
            target.addComponent(entity, body);
        });
    scene.registerComponent<Collider3D>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            Collider3D collider;
            collider.width = ComponentJson::floatValue(data, "Width", 1.0f);
            collider.height = ComponentJson::floatValue(data, "Height", 1.0f);
            collider.depth = ComponentJson::floatValue(data, "Depth", 1.0f);
            collider.offsetX = ComponentJson::floatValue(data, "OffsetX", 0.0f);
            collider.offsetY = ComponentJson::floatValue(data, "OffsetY", 0.0f);
            collider.offsetZ = ComponentJson::floatValue(data, "OffsetZ", 0.0f);
            collider.layer = static_cast<std::uint32_t>(ComponentJson::intValue(data, "Layer", 1));
            collider.mask = Physics3D::maskValue(data);
            collider.isTrigger = ComponentJson::boolValue(data, "IsTrigger", false);
            collider.rotates = ComponentJson::boolValue(data, "Rotates", false);
            target.addComponent(entity, collider);
        });
    scene.registerComponent<ArcWall3D>([](Scene &target, Entity entity, const nlohmann::json &data) {
        ArcWall3D wall;
        wall.radius = ComponentJson::floatValue(data, "Radius", 10.0f);
        if (wall.radius <= 0.0f)
            throw std::invalid_argument("ArcWall3D Radius must be positive");
        wall.startAngle = ComponentJson::floatValue(data, "StartAngle", 0.0f);
        wall.endAngle = ComponentJson::floatValue(data, "EndAngle", 90.0f);
        wall.height = ComponentJson::floatValue(data, "Height", 10.0f);
        wall.keepInside = ComponentJson::boolValue(data, "KeepInside", true);
        wall.reach = ComponentJson::floatValue(data, "Reach", 0.0f);
        target.addComponent(entity, wall);
    });
}

std::vector<PropertyDescriptor> Physics3D::settings() const
{
    return {{"GravityX", valueType::Float, "0"},
            {"GravityY", valueType::Float, "-9.81"},
            {"GravityZ", valueType::Float, "0"}};
}

float Physics3D::floatSetting(Scene &scene, const std::string &key)
{
    const std::string text = scene.getResource<ProjectSettings>().get("Physics3D", key);
    try {
        std::size_t parsed = 0;
        const float result = std::stof(text, &parsed);
        if (parsed != text.size())
            throw std::invalid_argument("trailing characters");
        return result;
    } catch (const std::exception &) {
        throw std::invalid_argument("Invalid Physics3D setting " + key + ": " + text);
    }
}

void Physics3D::build(Application &app)
{
    app._scene.addResource<Gravity3D>(Gravity3D{floatSetting(app._scene, "GravityX"),
                                                floatSetting(app._scene, "GravityY"),
                                                floatSetting(app._scene, "GravityZ")});

    app._scene.registerSystem<Gravity3DSystem>(FixedUpdate);
    ComponentSet gravity;
    gravity.set(app._scene.getComponentType<RigidBody3D>());
    gravity.set(app._scene.getComponentType<Velocity3D>());
    app._scene.setSystemComponentSet<Gravity3DSystem>(gravity);
    app._scene.setSystemAccess<Gravity3DSystem>(app._scene.access()
                                                    .read<RigidBody3D>()
                                                    .write<Velocity3D>()
                                                    .readResource<Gravity3D>());

    app._scene.registerSystem<Movement3DSystem>(FixedUpdate);
    ComponentSet movement;
    movement.set(app._scene.getComponentType<Transform>());
    movement.set(app._scene.getComponentType<Velocity3D>());
    app._scene.setSystemComponentSet<Movement3DSystem>(movement);
    app._scene.setSystemAccess<Movement3DSystem>(
        app._scene.access().read<Velocity3D>().write<Transform>());

    app._scene.registerSystem<Collider3DSystem>(FixedUpdate);
    ComponentSet collider;
    collider.set(app._scene.getComponentType<Transform>());
    collider.set(app._scene.getComponentType<Collider3D>());
    app._scene.setSystemComponentSet<Collider3DSystem>(collider);

    // After the boxes: the arcs have the last word on where a body ends up
    app._scene.registerSystem<ArcWall3DSystem>(FixedUpdate);
    ComponentSet arcWall;
    arcWall.set(app._scene.getComponentType<Transform>());
    arcWall.set(app._scene.getComponentType<ArcWall3D>());
    app._scene.setSystemComponentSet<ArcWall3DSystem>(arcWall);
}

void Physics3D::cleanup(Application &app) { (void)app; }
