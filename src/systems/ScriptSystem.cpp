/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** ScriptSystem
*/

#include "ScriptSystem.hpp"

void ScriptSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        Script &script = scene.getComponent<Script>(entity);
        if (script.update)
            script.update(scene, entity);
    }
}
