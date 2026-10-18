/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Prefab
*/

#include "Prefab.hpp"

Entity Prefab::instantiate(Scene &scene) const
{
    Entity entity = scene.createEntity();
    for (auto const &component : _components)
        component(scene, entity);
    return entity;
}
