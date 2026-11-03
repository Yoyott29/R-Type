/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UIPlugin
*/

#pragma once
#include "Application.hpp"

class UIPlugin : public IPlugin
{
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
};
