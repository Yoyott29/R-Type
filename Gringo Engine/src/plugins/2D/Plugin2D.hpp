/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Plugin2D
*/

#pragma once
#include "Application.hpp"

// 2D world: Sprite components (colored shapes or images) drawn through the project's renderer
class Plugin2D : public Plugin<Plugin2D>
{
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
    bool requiresRenderer() const override { return true; }
};
