/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SystemManager
*/

#include "SystemManager.hpp"

void SystemManager::entityDestroyed(Entity entity)
{
    for (auto const &pair : _systems)
        pair.second->_entities.erase(entity);
}

void SystemManager::entityComponentSetChanged(Entity entity, ComponentSet componentSet)
{
    for (auto const &pair : _systems) {
        auto requiredIterator = _componentSets.find(pair.first);
        if (requiredIterator == _componentSets.end())
            continue;

        ComponentSet const &requiredSet = requiredIterator->second;
        if ((componentSet & requiredSet) == requiredSet)
            pair.second->_entities.insert(entity);
        else
            pair.second->_entities.erase(entity);
    }
}
