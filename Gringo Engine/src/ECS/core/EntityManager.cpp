/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** EntityManager
*/

#include "EntityManager.hpp"
#include "EngineError.hpp"

EntityManager::EntityManager()
{
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
        _availableEntities.push(entity);
}

Entity EntityManager::createEntity()
{
    if (_livingEntityCount >= MAX_ENTITIES)
        throw EntityLimitError();

    const Entity id = _availableEntities.front();
    _availableEntities.pop();
    _livingEntityCount++;
    _alive.set(id);
    return id;
}

void EntityManager::destroyEntity(Entity entity)
{
    if (entity >= MAX_ENTITIES)
        throw EntityLimitError();
    if (!isAlive(entity))
        throw OutOfRangeEntityError();

    _componentSets[entity].reset();
    _availableEntities.push(entity);
    _livingEntityCount--;
    _alive.reset(entity);
}

std::vector<Entity> EntityManager::getLivingEntities() const
{
    std::vector<Entity> entities;
    entities.reserve(_livingEntityCount);
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
        if (isAlive(entity))
            entities.push_back(entity);
    return entities;
}

void EntityManager::setupComponentSet(Entity entity, ComponentSet componentSet)
{
    if (!isAlive(entity))
        throw OutOfRangeEntityError();

    _componentSets[entity] = componentSet;
}

ComponentSet EntityManager::getComponentSet(Entity entity) const
{
    if (!isAlive(entity))
        throw OutOfRangeEntityError();

    return _componentSets[entity];
}

bool EntityManager::isAlive(Entity entity) const
{
    return entity < MAX_ENTITIES && _alive.test(entity);
}

void EntityManager::putAllEntitysInDestroyQueue(std::queue<Entity> &destroyQueue)
{
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        if (isAlive(entity)) {
            destroyQueue.push(entity);
        }
    }
}
