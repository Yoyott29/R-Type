/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Scene
*/

#include "Scene.hpp"
#include "ChildOf.hpp"
#include "nlohmann/json.hpp"

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

    // Its children go with it, and theirs with them. Done right away, so a new entity reusing
    // this id cannot become their parent
    if (_componentManager->isRegistered<ChildOf>())
        for (Entity child : view<ChildOf>())
            if (getComponent<ChildOf>(child).parent == entity)
                destroyEntity(child);
}

void Scene::runSchedule(ScheduleLabel label) { _systemManager->runSchedule(label, *this); }

void Scene::addComponentFromJson(Entity entity, const nlohmann::json &component)
{
    validateEntity(entity);
    _componentManager->addComponentFromJson(*this, entity, component);
    ComponentSet componentSet = _entityManager->getComponentSet(entity);
    componentSet.set(_componentManager->getComponentTypeByName(component.value("name", "")));
    _entityManager->setupComponentSet(entity, componentSet);
    _systemManager->entityComponentSetChanged(entity, componentSet);
}

std::function<void(Scene &, Entity)>
Scene::componentInitializer(const nlohmann::json &component) const
{
    return _componentManager->componentInitializer(component);
}

void Scene::flushDestroyQueue()
{
    while (!_destroyQueue.empty()) {
        Entity entity = _destroyQueue.front();
        _destroyQueue.pop();
        destroyEntity(entity);
    }
}
