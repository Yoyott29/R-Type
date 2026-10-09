/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ComponentManager
*/

#pragma once
#include "ComponentPool.hpp"
#include "EngineError.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>

class ComponentManager
{
  public:
    template <typename Component> void registerComponent()
    {
        const std::type_index key(typeid(Component));

        if (_componentTypes.find(key) != _componentTypes.end()) {
            std::cout << "Component type registered more than once" << std::endl;
            return;
        }

        _componentTypes.insert({key, _nextComponentType});
        _componentPools.insert({key, std::make_unique<ComponentPool<Component>>()});
        _nextComponentType++;
    }

    template <typename Component> ComponentType getComponentType()
    {
        const std::type_index key(typeid(Component));

        if (_componentTypes.find(key) == _componentTypes.end())
            throw ComponentNotRegisteredError();

        return _componentTypes[key];
    }

    template <typename Component> void addComponent(Entity entity, Component component)
    {
        getComponentPool<Component>().insertComponent(entity, component);
    }

    template <typename Component> void removeComponent(Entity entity, Component component)
    {
        getComponentPool<Component>()->removeComponent(entity);
    }

    template <typename Component> Component &getComponent(Entity entity)
    {
        return getComponentPool<Component>().getComponent(entity);
    }

    void entityDestroyed(Entity entity)
    {
        for (auto const &pair : _componentPools)
            pair.second->destroyEntity(entity);
    }

  private:
    std::unordered_map<std::type_index, ComponentType> _componentTypes = {};
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> _componentPools = {};
    ComponentType _nextComponentType = {};

    template <typename Component> ComponentPool<Component> &getComponentPool()
    {
        const std::type_index key(typeid(Component));
        auto iterator = _componentPools.find(key);

        if (iterator == _componentPools.end())
            throw ComponentNotRegisteredError();
        return static_cast<ComponentPool<Component> &>(*iterator->second);
    }
};
