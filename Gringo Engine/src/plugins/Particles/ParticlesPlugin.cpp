/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticlesPlugin
*/

#include "ParticlesPlugin.hpp"
#include "ComponentJson.hpp"
#include "ParticleEmitter.hpp"
#include "ParticleEmitterSystem.hpp"
#include "ParticleRenderSystem.hpp"
#include "Transform.hpp"

void ParticlesPlugin::registerComponents(Scene &scene)
{
    // register the ParticleEmitter component, with how to read it from a .gringo file.
    // It starts stopped: a script calls start()
    scene.registerComponent<ParticleEmitter>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            ParticleEmitter emitter;
            emitter.density = ComponentJson::floatValue(data, "Density", 10.0f);
            if (ComponentJson::value(data, "Shape", "Circle") == "Box")
                emitter.shape = BoxShape{ComponentJson::floatValue(data, "Width", 4.0f),
                                         ComponentJson::floatValue(data, "Height", 4.0f)};
            else
                emitter.shape = CircleShape{ComponentJson::floatValue(data, "Radius", 2.0f)};
            emitter.lifetime = ComponentJson::floatValue(data, "Lifetime", 1.0f);
            emitter.speed = ComponentJson::floatValue(data, "Speed", 100.0f);
            emitter.color = ComponentJson::colorValue(data, "Color", {255, 255, 255, 255});
            target.addComponent(entity, emitter);
        });
}

void ParticlesPlugin::build(Application &app)
{
    ComponentSet requirements;
    requirements.set(app._scene.getComponentType<Transform>());
    requirements.set(app._scene.getComponentType<ParticleEmitter>());

    // spawns and moves the particles
    app._scene.registerSystem<ParticleEmitterSystem>(FixedUpdate);
    app._scene.setSystemComponentSet<ParticleEmitterSystem>(requirements);
    app._scene.setSystemAccess<ParticleEmitterSystem>(
        app._scene.access().read<Transform>().write<ParticleEmitter>());

    // draws them with the world
    app._scene.registerSystem<ParticleRenderSystem>(Render);
    app._scene.setSystemComponentSet<ParticleRenderSystem>(requirements);
}

void ParticlesPlugin::cleanup(Application &app) { (void)app; }
