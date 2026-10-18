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
      _systemManager(std::make_unique<SystemManager>()),
      _resourceManager(std::make_unique<ResourceManager>())
{
}

Entity Scene::createEntity() { return _entityManager->createEntity(); }

bool Scene::isEntityAlive(Entity entity) const { return _entityManager->isAlive(entity); }

void Scene::destroyEntity(Entity entity)
{
    if (!_entityManager->isAlive(entity))
        return;

    _entityManager->destroyEntity(entity);
    _componentManager->entityDestroyed(entity);
    _systemManager->entityDestroyed(entity);
}

void Scene::runSchedule(ScheduleLabel label) { _systemManager->runSchedule(label, *this); }

void Scene::flushDestroyQueue()
{
    while (!_destroyQueue.empty()) {
        Entity entity = _destroyQueue.front();
        _destroyQueue.pop();
        destroyEntity(entity);
    }
}
