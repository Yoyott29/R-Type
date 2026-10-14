/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** ScriptSystem
*/

#include "ScriptSystem.hpp"

void ScriptSystem::Start(Scene &scene)
{
    for (Entity entity : _entities) {
        Script &script = scene.getComponent<Script>(entity);
        if (script.timing == Timing::AT_LAUNCH && !script.isRunning) {
            script.isRunning = true;
            script.runScript(scene, entity);
        }
    }
}

void ScriptSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Script &script = scene.getComponent<Script>(entity);
        if (script.timing == Timing::ON_UPDATE)
            script.runScript(scene, entity);
    }
}
