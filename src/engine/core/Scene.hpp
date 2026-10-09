/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Scene
*/

#pragma once
#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"
#include <memory>

class Scene
{
  public:
    Scene();
    Entity createEntity();
    void destroyEntity(Entity entity);

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

    template <typename SystemType> std::shared_ptr<SystemType> registerSystem()
    {
        return _systemManager->registerSystem<SystemType>();
    }

    template <typename SystemType> void setSystemComponentSet(ComponentSet componentSet)
    {
        _systemManager->setComponentSet<SystemType>(componentSet);
    }

    template <typename Component> ComponentType getComponentType()
    {
        return _componentManager->getComponentType<Component>();
    }

  private:
    std::unique_ptr<EntityManager> _entityManager;
    std::unique_ptr<ComponentManager> _componentManager;
    std::unique_ptr<SystemManager> _systemManager;
};
