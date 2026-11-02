/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** PrefabsManager
*/

#include "PrefabsManager.hpp"

void PrefabsManager::createPrefab(std::string const &name, Prefab const &prefab)
{
    _prefabs[name] = prefab;
}

Entity PrefabsManager::InstantiatePrefab(std::string const &name, Scene &scene)
{
    return _prefabs[name].instantiate(scene);
}
