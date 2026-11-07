/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** PrefabInstance
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include <string>

// Set on a scene entity: when the game starts, a copy of this prefab is created there. Empty: none
struct PrefabInstance {
    std::string prefab;
};

template <> struct ComponentDescription<PrefabInstance> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Prefab", {{"Prefab", valueType::Prefab, NO_FILE}}, {52, 62, 70, 255}, ""};
    }
};
