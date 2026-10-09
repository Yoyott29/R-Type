/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Scene
*/

#include "Scene.hpp"

Scene::Scene()
    : _entityManager(std::make_unique<EntityManager>()),
    _componentManager(std::make_unique<ComponentManager>()),
    _systemManager(std::make_unique<SystemManager>())
{
}

Entity Scene::createEntity()
{
    return _entityManager->createEntity();
}

void Scene::destroyEntity(Entity entity)
{
    if (!_entityManager->isAlive(entity))
        return;

    _entityManager->destroyEntity(entity);
    _componentManager->entityDestroyed(entity);
    _systemManager->entityDestroyed(entity);
}
