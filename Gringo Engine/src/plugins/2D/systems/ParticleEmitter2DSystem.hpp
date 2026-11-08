/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleEmitter2DSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Runs in FixedUpdate: spawns the particles of the started emitters, moves and ages them all
class ParticleEmitter2DSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    // A random number from min to max, with std::rand
    static float random(float min, float max);
};
