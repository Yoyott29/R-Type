/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleEmitter3DSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// The slowest particle leaves at this part of the emitter's speed, the fastest at all of it
#define MIN_SPEED_PART 0.3f

// Runs in FixedUpdate: spawns the particles of the started 3D emitters, moves and ages them all
class ParticleEmitter3DSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    // A random number from min to max, with std::rand
    static float random(float min, float max);
    // A random direction, of length 1
    static void randomDirection(float &x, float &y, float &z);
};
