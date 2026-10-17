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
        for (std::size_t i = 0; i < scene.getComponentCount<Script>(entity); i++) {
            Script &script = scene.getComponent<Script>(entity, i);
            if (script.timing == Timing::AT_LAUNCH && !script.isRunning) {
                script.isRunning = true;
                auto run = script.runScript;
                run(scene, entity);
            }
        }
    }
}

void ScriptSystem::update(Scene &scene)
{
    Start(scene);
    for (Entity entity : _entities) {
        for (std::size_t i = 0; i < scene.getComponentCount<Script>(entity); i++) {
            Script &script = scene.getComponent<Script>(entity, i);
            if (script.timing == Timing::ON_UPDATE) {
                auto run = script.runScript;
                run(scene, entity);
            }
        }
    }
}
