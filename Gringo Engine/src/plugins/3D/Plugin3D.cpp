/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Plugin3D
*/

#include "Plugin3D.hpp"
#include "ActiveCamera3D.hpp"
#include "Camera.hpp"
#include "CameraFollow3D.hpp"
#include "CameraSystem.hpp"
#include "CameraFollowSystem.hpp"
#include "ComponentJson.hpp"
#include "Model3D.hpp"
#include "Model3DSystem.hpp"
#include "ParticleEmitter3D.hpp"
#include "ParticleEmitter3DSystem.hpp"
#include "ParticleRender3DSystem.hpp"
#include "ProjectAssets.hpp"

void Plugin3D::registerComponents(Scene &scene)
{
    scene.registerComponent<Camera>([](Scene &target, Entity entity, const nlohmann::json &data) {
        Camera camera;
        camera.targetX = ComponentJson::floatValue(data, "TargetX", 0.0f);
        camera.targetY = ComponentJson::floatValue(data, "TargetY", 0.0f);
        camera.targetZ = ComponentJson::floatValue(data, "TargetZ", 0.0f);
        camera.upX = ComponentJson::floatValue(data, "UpX", 0.0f);
        camera.upY = ComponentJson::floatValue(data, "UpY", 1.0f);
        camera.upZ = ComponentJson::floatValue(data, "UpZ", 0.0f);
        camera.fieldOfView = ComponentJson::floatValue(data, "FieldOfView", 45.0f);
        camera.active = ComponentJson::boolValue(data, "Active", true);
        target.addComponent(entity, camera);
    });
    scene.registerComponent<CameraFollow3D>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            CameraFollow3D follow;
            follow.distance = ComponentJson::floatValue(data, "Distance", 8.0f);
            follow.height = ComponentJson::floatValue(data, "Height", 3.0f);
            follow.targetHeight = ComponentJson::floatValue(data, "TargetHeight", 1.0f);
            target.addComponent(entity, follow);
        });
    scene.registerComponent<Model3D>([](Scene &target, Entity entity, const nlohmann::json &data) {
        Model3D model;
        const std::string path = ComponentJson::value(data, "ModelPath", "");
        model.primitive = ComponentJson::value(data, "Primitive", path.empty() ? "Cube" : "Model");
        if (!path.empty() && path != NO_FILE)
            model.path = ProjectAssets::resolve(target, path);
        const std::string texture = ComponentJson::value(data, "TexturePath", "");
        if (!texture.empty() && texture != NO_FILE)
            model.texturePath = ProjectAssets::resolve(target, texture);
        model.color = ComponentJson::colorValue(data, "Color", {255, 255, 255, 255});
        model.width = ComponentJson::floatValue(data, "Width", 1.0f);
        model.height = ComponentJson::floatValue(data, "Height", 1.0f);
        model.depth = ComponentJson::floatValue(data, "Depth", 1.0f);
        model.scale = ComponentJson::floatValue(data, "Scale", 1.0f);
        model.animation = ComponentJson::intValue(data, "Animation", -1);
        model.animationSpeed = ComponentJson::floatValue(data, "AnimationSpeed", 1.0f);
        target.addComponent(entity, std::move(model));
    });
    // register the ParticleEmitter3D component. It starts stopped: a script calls start()
    scene.registerComponent<ParticleEmitter3D>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            ParticleEmitter3D emitter;
            emitter.density = ComponentJson::floatValue(data, "Density", 10.0f);
            if (ComponentJson::value(data, "Shape", "Cube") == "Sphere")
                emitter.shape = SphereShape{ComponentJson::floatValue(data, "Radius", 0.1f)};
            else
                emitter.shape = CubeShape{ComponentJson::floatValue(data, "Width", 0.2f),
                                          ComponentJson::floatValue(data, "Height", 0.2f),
                                          ComponentJson::floatValue(data, "Depth", 0.2f)};
            emitter.lifetime = ComponentJson::floatValue(data, "Lifetime", 1.0f);
            emitter.speed = ComponentJson::floatValue(data, "Speed", 1.0f);
            emitter.spawnRadius = ComponentJson::floatValue(data, "SpawnRadius", 0.0f);
            emitter.color = ComponentJson::colorValue(data, "Color", {255, 255, 255, 255});
            target.addComponent(entity, emitter);
        });
}

void Plugin3D::build(Application &app)
{
    app._scene.addResource<ActiveCamera3D>();
    app._scene.registerSystem<CameraSystem>(Render);
    ComponentSet cameraRequirements;
    cameraRequirements.set(app._scene.getComponentType<Camera>());
    cameraRequirements.set(app._scene.getComponentType<Transform>());
    app._scene.setSystemComponentSet<CameraSystem>(cameraRequirements);

    app._scene.registerSystem<CameraFollowSystem>(PostUpdate);
    ComponentSet followRequirements;
    followRequirements.set(app._scene.getComponentType<Camera>());
    followRequirements.set(app._scene.getComponentType<CameraFollow3D>());
    followRequirements.set(app._scene.getComponentType<ChildOf>());
    followRequirements.set(app._scene.getComponentType<Transform>());
    app._scene.setSystemComponentSet<CameraFollowSystem>(followRequirements);

    app._scene.registerSystem<Model3DSystem>(Render);
    ComponentSet modelRequirements;
    modelRequirements.set(app._scene.getComponentType<Model3D>());
    modelRequirements.set(app._scene.getComponentType<Transform>());
    app._scene.setSystemComponentSet<Model3DSystem>(modelRequirements);

    // 3D particles: spawned and moved in FixedUpdate, drawn after the models (they fade out)
    ComponentSet particleRequirements;
    particleRequirements.set(app._scene.getComponentType<Transform>());
    particleRequirements.set(app._scene.getComponentType<ParticleEmitter3D>());
    app._scene.registerSystem<ParticleEmitter3DSystem>(FixedUpdate);
    app._scene.setSystemComponentSet<ParticleEmitter3DSystem>(particleRequirements);
    app._scene.registerSystem<ParticleRender3DSystem>(Render);
    app._scene.setSystemComponentSet<ParticleRender3DSystem>(particleRequirements);
}

void Plugin3D::cleanup(Application &app) { (void)app; }
