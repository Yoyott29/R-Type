/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleEmitter3DSystem
*/

#include "ParticleEmitter3DSystem.hpp"
#include "ParticleEmitter3D.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <numbers>

float ParticleEmitter3DSystem::random(float min, float max)
{
    return min + (max - min) * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));
}

void ParticleEmitter3DSystem::randomDirection(float &x, float &y, float &z)
{
    // A random height on the unit sphere and a random angle around it
    y = random(-1.0f, 1.0f);
    const float angle = random(0.0f, 2.0f * std::numbers::pi_v<float>);
    const float around = std::sqrt(1.0f - y * y);
    x = std::cos(angle) * around;
    z = std::sin(angle) * around;
}

void ParticleEmitter3DSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        const Transform &position = scene.getComponent<Transform>(entity);
        ParticleEmitter3D &emitter = scene.getComponent<ParticleEmitter3D>(entity);

        for (Particle3D &particle : emitter.particles) {
            particle.x += particle.dx * FIXED_DELTA_TIME;
            particle.y += particle.dy * FIXED_DELTA_TIME;
            particle.z += particle.dz * FIXED_DELTA_TIME;
            particle.life -= FIXED_DELTA_TIME;
        }
        std::erase_if(emitter.particles,
                      [](const Particle3D &particle) { return particle.life <= 0.0f; });

        if (!emitter.emitting)
            continue;
        // Each particle starts anywhere in the spawn ball and leaves in a random direction at a
        // random part of the speed, so they scatter
        emitter.spawnAccumulator += std::max(emitter.density, 0.0f) * FIXED_DELTA_TIME;
        for (; emitter.spawnAccumulator >= 1.0f; emitter.spawnAccumulator -= 1.0f) {
            float startX = 0.0f;
            float startY = 0.0f;
            float startZ = 0.0f;
            // The cube root spreads the points evenly in the ball instead of crowding the centre
            randomDirection(startX, startY, startZ);
            const float distance = emitter.spawnRadius * std::cbrt(random(0.0f, 1.0f));
            float dx = 0.0f;
            float dy = 0.0f;
            float dz = 0.0f;
            randomDirection(dx, dy, dz);
            const float speed = emitter.speed * random(MIN_SPEED_PART, 1.0f);
            emitter.particles.push_back({position.x + startX * distance,
                                         position.y + startY * distance,
                                         position.z + startZ * distance, dx * speed, dy * speed,
                                         dz * speed, emitter.lifetime});
        }
    }
}
