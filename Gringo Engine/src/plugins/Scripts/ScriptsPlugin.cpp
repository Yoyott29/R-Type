/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ScriptsPlugin
*/

#include "ScriptsPlugin.hpp"
#include "Script.hpp"
#include "ScriptSystem.hpp"

void ScriptsPlugin::registerComponents(Scene &scene)
{
    // No deserializer: the game's generated main creates each script from its Path
    scene.registerComponent<Script>();
}

void ScriptsPlugin::build(Application &app)
{
    app._scene.registerSystem<ScriptSystem>(FixedUpdate);
    ComponentSet scriptRequirements;
    scriptRequirements.set(app._scene.getComponentType<Script>());
    app._scene.setSystemComponentSet<ScriptSystem>(scriptRequirements);
}

void ScriptsPlugin::cleanup(Application &app) { (void)app; }
