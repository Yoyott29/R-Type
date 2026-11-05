/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleEmitterSystem
*/

#include "ParticleEmitterSystem.hpp"
#include "ParticleEmitter.hpp"
#include "Transform.hpp"
#include "Time.hpp"
#include <algorithm>
#include <cmath>
#include <numbers>

void ParticleEmitterSystem::update(Scene &scene)
{
    std::uniform_real_distribution<float> angle(0.0f, 2.0f * std::numbers::pi_v<float>);

    for (Entity entity : _entities) {
        Transform const &position = scene.getComponent<Transform>(entity);
        ParticleEmitter &emitter = scene.getComponent<ParticleEmitter>(entity);

        for (Particle &particle : emitter.particles) {
            particle.x += particle.dx * FIXED_DELTA_TIME;
            particle.y += particle.dy * FIXED_DELTA_TIME;
            particle.life -= FIXED_DELTA_TIME;
        }
        std::erase_if(emitter.particles, [](Particle const &particle) { return particle.life <= 0.0f; });

        if (!emitter.emitting)
            continue;
        // Each particle leaves the emitter in a random direction
        emitter.spawnAccumulator += std::max(emitter.density, 0.0f) * FIXED_DELTA_TIME;
        for (; emitter.spawnAccumulator >= 1.0f; emitter.spawnAccumulator -= 1.0f) {
            const float direction = angle(_random);
            emitter.particles.push_back({position.x, position.y,
                                         std::cos(direction) * emitter.speed,
                                         std::sin(direction) * emitter.speed, emitter.lifetime});
        }
    }
}
