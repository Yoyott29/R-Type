/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ScriptsPlugin
*/

#pragma once
#include "Application.hpp"

// Entity scripts: the Script component (a GringoEntityScript per entity, several allowed) and the
// ScriptSystem calling their onStart and onUpdate every fixed tick
class ScriptsPlugin : public Plugin<ScriptsPlugin>
{
  public:
    void registerComponents(Scene &scene) override;
    void build(Application &app) override;
    void cleanup(Application &app) override;
};
