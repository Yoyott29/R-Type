/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleRender2DSystem
*/

#include "ParticleRender2DSystem.hpp"
#include "IRenderer.hpp"
#include "ParticleEmitter2D.hpp"
#include <algorithm>
#include <functional>
#include <type_traits>
#include <variant>

void ParticleRender2DSystem::update(Scene &scene)
{
    // Without the renderer plugin there is no window to draw in
    if (!scene.hasResource<std::reference_wrapper<IRenderer>>())
        return;
    IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();

    for (Entity entity : _entities) {
        ParticleEmitter2D const &emitter = scene.getComponent<ParticleEmitter2D>(entity);
        for (Particle2D const &particle : emitter.particles) {
            ColorRGBA color = emitter.color;
            const float remaining =
                emitter.lifetime > 0.0f ? particle.life / emitter.lifetime : 0.0f;
            color.a = static_cast<decltype(color.a)>(static_cast<float>(color.a) *
                                                     std::clamp(remaining, 0.0f, 1.0f));
            std::visit(
                [&](auto const &shape) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, CircleShape>)
                        renderer.drawCircle(particle.x, particle.y, shape.radius, color);
                    else
                        renderer.drawRectangle(particle.x, particle.y, shape.width, shape.height,
                                               color);
                },
                emitter.shape);
        }
    }
}
