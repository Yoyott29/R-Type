/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleEmitter2DSystem
*/

#include "ParticleEmitter2DSystem.hpp"
#include "ParticleEmitter2D.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <numbers>

float ParticleEmitter2DSystem::random(float min, float max)
{
    return min + (max - min) * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
}

void ParticleEmitter2DSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Transform const &position = scene.getComponent<Transform>(entity);
        ParticleEmitter2D &emitter = scene.getComponent<ParticleEmitter2D>(entity);

        for (Particle2D &particle : emitter.particles) {
            particle.x += particle.dx * FIXED_DELTA_TIME;
            particle.y += particle.dy * FIXED_DELTA_TIME;
            particle.life -= FIXED_DELTA_TIME;
        }
        std::erase_if(emitter.particles,
                      [](Particle2D const &particle) { return particle.life <= 0.0f; });

        if (!emitter.emitting)
            continue;
        // Each particle leaves the emitter in a random direction
        emitter.spawnAccumulator += std::max(emitter.density, 0.0f) * FIXED_DELTA_TIME;
        for (; emitter.spawnAccumulator >= 1.0f; emitter.spawnAccumulator -= 1.0f) {
            const float direction = random(0.0f, 2.0f * std::numbers::pi_v<float>);
            emitter.particles.push_back({position.x, position.y,
                                         std::cos(direction) * emitter.speed,
                                         std::sin(direction) * emitter.speed, emitter.lifetime});
        }
    }
}
