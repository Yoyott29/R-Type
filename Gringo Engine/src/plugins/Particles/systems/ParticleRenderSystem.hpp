/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleRenderSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Runs in the Render schedule: draws the particles of every emitter, fading out with their life
class ParticleRenderSystem : public System
{
  public:
    void update(Scene &scene) override;
};
