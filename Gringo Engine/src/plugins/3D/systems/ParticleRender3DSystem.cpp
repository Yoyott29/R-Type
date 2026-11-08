/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleRender3DSystem
*/

#include "ParticleRender3DSystem.hpp"
#include "ActiveCamera3D.hpp"
#include "IRenderer.hpp"
#include "ParticleEmitter3D.hpp"
#include <algorithm>
#include <functional>
#include <type_traits>
#include <variant>

void ParticleRender3DSystem::update(Scene &scene)
{
    // Without the renderer plugin there is no window to draw in
    if (!scene.hasResource<std::reference_wrapper<IRenderer>>())
        return;
    IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();
    const RendererCamera3D &camera = scene.getResource<ActiveCamera3D>().value;

    for (Entity entity : _entities) {
        const ParticleEmitter3D &emitter = scene.getComponent<ParticleEmitter3D>(entity);
        for (const Particle3D &particle : emitter.particles) {
            const float remaining =
                emitter.lifetime > 0.0f ? particle.life / emitter.lifetime : 0.0f;
            RendererModel3D model;
            model.color = emitter.color;
            model.color.a =
                static_cast<unsigned char>(emitter.color.a * std::clamp(remaining, 0.0f, 1.0f));
            std::visit(
                [&model](const auto &shape) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, SphereShape>) {
                        model.primitive = "Sphere";
                        model.scale = shape.radius;
                    } else {
                        model.primitive = "Cube";
                        model.width = shape.width;
                        model.height = shape.height;
                        model.depth = shape.depth;
                    }
                },
                emitter.shape);
            renderer.drawModel3D(camera, model, particle.x, particle.y, particle.z);
        }
    }
}
