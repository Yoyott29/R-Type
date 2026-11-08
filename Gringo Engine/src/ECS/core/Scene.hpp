/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Scene
*/

#pragma once
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "ParallelFor.hpp"
#include "ResourceManager.hpp"
#include "SystemManager.hpp"
#include <memory>
#include <mutex>
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
        const std::lock_guard lock(_destroyMutex);
        _entityManager->putAllEntitysInDestroyQueue(_destroyQueue);
    };

    template <typename Component> void registerComponent()
    {
        _componentManager->registerComponent<Component>();
    }

    template <typename Component>
    // Plugins can provide this callback to deserialize their components from .gringo data.
    void registerComponent(ComponentManager::ComponentDeserializer deserializer)
    {
        _componentManager->registerComponent<Component>(std::move(deserializer));
    }

    const std::vector<ComponentDescriptor> &getComponentDescriptors() const
    {
        return _componentManager->getDescriptors();
    }

    template <typename Component> void addComponent(Entity entity, Component component)
    {
        validateEntity(entity);
        _componentManager->addComponent<Component>(entity, component);

        ComponentSet componentSet = _entityManager->getComponentSet(entity);
        componentSet.set(_componentManager->getComponentType<Component>(), true);
        _entityManager->setupComponentSet(entity, componentSet);

        _systemManager->entityComponentSetChanged(entity, componentSet);
    }

    template <typename Component> void removeComponent(Entity entity)
    {
        validateEntity(entity);
        _componentManager->removeComponent<Component>(entity);

        ComponentSet componentSet = _entityManager->getComponentSet(entity);
        componentSet.set(_componentManager->getComponentType<Component>(), false);
        _entityManager->setupComponentSet(entity, componentSet);

        _systemManager->entityComponentSetChanged(entity, componentSet);
    }

    // False if the entity is dead or the component type was never registered
    template <typename Component> bool hasComponent(Entity entity) const
    {
        if (!_entityManager->isAlive(entity) || !_componentManager->isRegistered<Component>())
            return false;
        return _entityManager->getComponentSet(entity).test(
            _componentManager->getComponentType<Component>());
    }

    template <typename Component> Component &getComponent(Entity entity)
    {
        validateEntity(entity);
        return _componentManager->getComponent<Component>(entity);
    }

    template <typename Component> Component &getComponent(Entity entity, std::size_t componentIndex)
    {
        validateEntity(entity);
        return _componentManager->getComponent<Component>(entity, componentIndex);
    }

    template <typename Component> std::size_t getComponentCount(Entity entity)
    {
        validateEntity(entity);
        return _componentManager->getComponentCount<Component>(entity);
    }

    void addComponentFromJson(Entity entity, const nlohmann::json &component);
    std::function<void(Scene &, Entity)>
    componentInitializer(const nlohmann::json &component) const;

    template <typename... Components> std::vector<Entity> view() const
    {
        std::vector<Entity> result;
        const auto entities = _entityManager->getLivingEntities();
        for (Entity entity : entities) {
            const ComponentSet components = _entityManager->getComponentSet(entity);
            if ((components & componentSet<Components...>()) == componentSet<Components...>())
                result.push_back(entity);
        }
        return result;
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

    SystemAccess access() const { return SystemAccess(*_componentManager); }

    template <typename SystemType> void setSystemAccess(SystemAccess access)
    {
        _systemManager->setAccess<SystemType>(std::move(access));
    }

    template <typename SystemType> void removeSystem()
    {
        _systemManager->removeSystem<SystemType>();
    }

    template <typename Component> ComponentType getComponentType() const
    {
        return _componentManager->getComponentType<Component>();
    }
    void queueDestroy(Entity entity)
    {
        const std::lock_guard lock(_destroyMutex);
        _destroyQueue.push(entity);
    }
    void flushDestroyQueue();

    // Runs body over [0, count) in chunks, on the pool when count exceeds minChunk. See parallelFor
    void parallelFor(std::size_t count, const ChunkBody &body,
                     std::size_t minChunk = PARALLEL_FOR_MIN_CHUNK)
    {
        if (count == 0)
            return;
        if (count <= minChunk) {
            body(0, count);
            return;
        }
        ::parallelFor(_systemManager->pool(), count, minChunk, body);
    }

    // body(const Item &) for each item, in parallel
    template <typename Item, typename Body>
    void parallelFor(const std::vector<Item> &items, Body body)
    {
        parallelFor(items.size(), [&items, &body](std::size_t begin, std::size_t end) {
            for (std::size_t i = begin; i < end; ++i)
                body(items[i]);
        });
    }

    bool hasThreadPool() const { return _systemManager->hasPool(); }

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

  private:
    void validateEntity(Entity entity) const
    {
        if (!_entityManager->isAlive(entity))
            throw OutOfRangeEntityError();
    }

    template <typename Component> ComponentSet componentSet() const
    {
        ComponentSet result;
        result.set(_componentManager->getComponentType<Component>());
        return result;
    }

    template <typename Component, typename Next, typename... Rest>
    ComponentSet componentSet() const
    {
        ComponentSet result = componentSet<Next, Rest...>();
        result.set(_componentManager->getComponentType<Component>());
        return result;
    }
    std::unique_ptr<EntityManager> _entityManager;
    std::unique_ptr<ComponentManager> _componentManager;
    std::unique_ptr<SystemManager> _systemManager;
    std::unique_ptr<ResourceManager> _resourceManager;
    std::queue<Entity> _destroyQueue;
    std::mutex _destroyMutex;
};
