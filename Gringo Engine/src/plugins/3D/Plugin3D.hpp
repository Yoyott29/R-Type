/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Plugin3D
*/

#pragma once
#include "Application.hpp"

class Plugin3D : public Plugin<Plugin3D> {
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
    bool requiresRenderer() const override { return true; }
};
