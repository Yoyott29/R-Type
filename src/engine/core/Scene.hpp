/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Scene
*/

#pragma once
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "ResourceManager.hpp"
#include "Script.hpp"
#include "SystemManager.hpp"
#include "ThreadPool.hpp"
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

class Scene
{
  public:
    Scene();
    Entity createEntity();
    bool isEntityAlive(Entity entity) const;
    void destroyEntity(Entity entity);
    void runSchedule(ScheduleLabel label);
    void putAllEntitysInDestroyQueue()
    {
        _entityManager->putAllEntitysInDestroyQueue(_destroyQueue);
    };

    template <typename Component> void registerComponent()
    {
        _componentManager->registerComponent<Component>();
    }

    template <typename Component> void addComponent(Entity entity, Component component)
    {
        _componentManager->addComponent<Component>(entity, component);

        ComponentSet componentSet = _entityManager->getComponentSet(entity);
        componentSet.set(_componentManager->getComponentType<Component>(), true);
        _entityManager->setupComponentSet(entity, componentSet);

        _systemManager->entityComponentSetChanged(entity, componentSet);
    }

    template <typename Component> void removeComponent(Entity entity)
    {
        _componentManager->removeComponent<Component>(entity);

        ComponentSet componentSet = _entityManager->getComponentSet(entity);
        componentSet.set(_componentManager->getComponentType<Component>(), false);
        _entityManager->setupComponentSet(entity, componentSet);

        _systemManager->entityComponentSetChanged(entity, componentSet);
    }

    template <typename Component> Component &getComponent(Entity entity)
    {
        return _componentManager->getComponent<Component>(entity);
    }

    template <typename Component> Component &getComponent(Entity entity, std::size_t componentIndex)
    {
        return _componentManager->getComponent<Component>(entity, componentIndex);
    }

    template <typename Component> std::size_t getComponentCount(Entity entity)
    {
        return _componentManager->getComponentCount<Component>(entity);
    }

    template <typename Component> std::vector<Component> &getComponents(Entity entity)
    {
        return _componentManager->getComponents<Component>(entity);
    }

    template <typename SystemType>
    std::shared_ptr<SystemType> registerSystem(ScheduleLabel label = Update)
    {
        return _systemManager->registerSystem<SystemType>(label);
    }

    template <typename SystemType> void setSystemComponentSet(ComponentSet componentSet)
    {
        _systemManager->setComponentSet<SystemType>(componentSet);
    }

    template <typename SystemType> void removeSystem()
    {
        _systemManager->removeSystem<SystemType>();
    }

    template <typename Component> ComponentType getComponentType()
    {
        return _componentManager->getComponentType<Component>();
    }
    void queueDestroy(Entity entity) { _destroyQueue.push(entity); }
    void flushDestroyQueue();

    template <typename Resource, typename... Args> Resource &addResource(Args &&...args)
    {
        return _resourceManager->addResource<Resource>(std::forward<Args>(args)...);
    }

    template <typename Resource> Resource &getResource()
    {
        return _resourceManager->getResource<Resource>();
    }

    template <typename Resource> bool hasResource() const
    {
        return _resourceManager->hasResource<Resource>();
    }

    template <typename Resource> void removeResource()
    {
        _resourceManager->removeResource<Resource>();
    }

    ThreadPool &getThreadPool() { return *_threadPool; }

  private:
    std::unique_ptr<EntityManager> _entityManager;
    std::unique_ptr<ComponentManager> _componentManager;
    std::unique_ptr<SystemManager> _systemManager;
    std::unique_ptr<ResourceManager> _resourceManager;
    std::unique_ptr<ThreadPool> _threadPool;
    std::queue<Entity> _destroyQueue;
};
