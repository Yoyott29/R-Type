/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** TryInstantiate
*/

#pragma once
#include "EngineError.hpp"
#include "NetLog.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include <optional>
#include <string>

// The new instance of the prefab, or nullopt (logged once) if the prefab is unknown or the
// entity limit is reached; `what` names the instance in the log, e.g. "mirror"
inline std::optional<Entity> tryInstantiate(Scene &scene, std::string const &name, LogOnce &log,
                                            std::string const &what)
{
    if (!scene.hasResource<PrefabsManager>() ||
        !scene.getResource<PrefabsManager>().hasPrefab(name)) {
        log("prefab" + name, "unknown prefab \"" + name + "\"");
        return std::nullopt;
    }
    try {
        return scene.getResource<PrefabsManager>().InstantiatePrefab(name, scene);
    } catch (EntityLimitError const &error) {
        log("entityLimit", what + " not created: " + error.what());
        return std::nullopt;
    }
}
