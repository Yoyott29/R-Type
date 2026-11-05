/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleEmitterSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"
#include <random>

// Runs in FixedUpdate: spawns the particles of the started emitters, moves and ages them all
class ParticleEmitterSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    std::mt19937 _random{std::random_device{}()};
};
