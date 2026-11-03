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
#include "SystemManager.hpp"
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

    template <typename Component> std::vector<Component> &getComponents(Entity entity)
    {
        validateEntity(entity);
        return _componentManager->getComponents<Component>(entity);
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

    template <typename SystemType> void removeSystem()
    {
        _systemManager->removeSystem<SystemType>();
    }

    template <typename Component> ComponentType getComponentType() const
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
};
