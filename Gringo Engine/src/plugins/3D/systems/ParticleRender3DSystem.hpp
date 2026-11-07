/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticleRender3DSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Runs in the Render schedule, after the models: draws the particles of every 3D emitter as
// cubes seen by the active camera, growing and fading out with their life
class ParticleRender3DSystem : public System
{
  public:
    void update(Scene &scene) override;
};
