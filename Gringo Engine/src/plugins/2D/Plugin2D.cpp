/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Plugin2D
*/

#include "Plugin2D.hpp"
#include "AnimationLibrary.hpp"
#include "AnimationSystem.hpp"
#include "Animator.hpp"
#include "ComponentJson.hpp"
#include "ParticleEmitter2D.hpp"
#include "ParticleEmitter2DSystem.hpp"
#include "ParticleRender2DSystem.hpp"
#include "Transform.hpp"
#include "ProjectAssets.hpp"
#include "SpriteSystem.hpp"
#include "Sprite.hpp"

void Plugin2D::registerComponents(Scene &scene)
{
    // register the Sprite component, with how to read it from a .gringo file
    scene.registerComponent<Sprite>([](Scene &target, Entity entity, const nlohmann::json &data) {
        Sprite sprite;
        if (ComponentJson::value(data, "Shape", "Box") == "Circle")
            sprite.shape = CircleShape{ComponentJson::floatValue(data, "Radius", 16.0f)};
        else
            sprite.shape = BoxShape{ComponentJson::floatValue(data, "Width", 32.0f),
                                    ComponentJson::floatValue(data, "Height", 32.0f)};
        sprite.drawOrder = ComponentJson::intValue(data, "DrawOrder", 0);
        const std::string texture = ComponentJson::value(data, "TexturePath", "");
        // No file chosen: the shape is drawn in its color ("default.png" in older projects)
        const bool noTexture = texture.empty() || texture == NO_FILE || texture == "default.png";
        sprite.texturePath = noTexture ? "" : ProjectAssets::resolve(target, texture);
        sprite.color = ComponentJson::colorValue(data, "Color", {255, 255, 255, 255});
        target.addComponent(entity, sprite);
    });
    // register the Animator component: the .anim file of the animation it starts with
    scene.registerComponent<Animator>([](Scene &target, Entity entity, const nlohmann::json &data) {
        Animator animator;
        const std::string path = ComponentJson::value(data, "Animation", "");
        if (!path.empty() && path != NO_FILE)
            animator.path = ProjectAssets::resolve(target, path);
        animator.speed = ComponentJson::floatValue(data, "Speed", 1.0f);
        target.addComponent(entity, animator);
    });
    // register the ParticleEmitter2D component. It starts stopped: a script calls start()
    scene.registerComponent<ParticleEmitter2D>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            ParticleEmitter2D emitter;
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

void Plugin2D::build(Application &app)
{
    // setup the SpriteSystem, drawn in the Render schedule
    app._scene.registerSystem<SpriteSystem>(Render);
    ComponentSet spriteRequirements;
    spriteRequirements.set(app._scene.getComponentType<Transform>());
    spriteRequirements.set(app._scene.getComponentType<Sprite>());
    app._scene.setSystemComponentSet<SpriteSystem>(spriteRequirements);

    // setup the AnimationSystem: it sets the Sprite of every Animator to its current frame
    app._scene.addResource<AnimationLibrary>();
    app._scene.registerSystem<AnimationSystem>(FixedUpdate);
    ComponentSet animationRequirements;
    animationRequirements.set(app._scene.getComponentType<Animator>());
    animationRequirements.set(app._scene.getComponentType<Sprite>());
    app._scene.setSystemComponentSet<AnimationSystem>(animationRequirements);
    // The library caches every clip it loads, so it is written
    app._scene.setSystemAccess<AnimationSystem>(app._scene.access()
                                                    .write<Animator>()
                                                    .write<Sprite>()
                                                    .writeResource<AnimationLibrary>()
                                                    .readResource<ProjectAssets>());

    // setup the 2D particles: spawned and moved in FixedUpdate, drawn with the world
    ComponentSet particleRequirements;
    particleRequirements.set(app._scene.getComponentType<Transform>());
    particleRequirements.set(app._scene.getComponentType<ParticleEmitter2D>());
    app._scene.registerSystem<ParticleEmitter2DSystem>(FixedUpdate);
    app._scene.setSystemComponentSet<ParticleEmitter2DSystem>(particleRequirements);
    app._scene.setSystemAccess<ParticleEmitter2DSystem>(
        app._scene.access().read<Transform>().write<ParticleEmitter2D>());
    app._scene.registerSystem<ParticleRender2DSystem>(Render);
    app._scene.setSystemComponentSet<ParticleRender2DSystem>(particleRequirements);
}

void Plugin2D::cleanup(Application &app) { (void)app; }
