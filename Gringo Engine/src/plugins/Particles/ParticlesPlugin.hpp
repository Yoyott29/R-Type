/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ParticlesPlugin
*/

#pragma once
#include "Application.hpp"

// ParticleEmitter components: particles spawned around an entity, started and stopped by scripts
class ParticlesPlugin : public Plugin<ParticlesPlugin>
{
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
    bool requiresRenderer() const override { return true; }
};
