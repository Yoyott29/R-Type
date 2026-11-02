/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ComponentManager
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include "ComponentPool.hpp"
#include "EngineError.hpp"
#include <memory>
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

class Scene;

class ComponentManager
{
  public:
    using ComponentDeserializer =
        std::function<void(Scene &, Entity, const nlohmann::json &)>;

    // Idempotent: every plugin registers the components it uses, a second call is a no-op
    template <typename Component> void registerComponent()
    {
        const std::type_index key(typeid(Component));

        auto existing = _componentTypes.find(key);
        if (existing != _componentTypes.end()) {
            if constexpr (ComponentDescription<Component>::isDescribed)
                _componentNames.insert_or_assign(ComponentDescription<Component>::describe().name,
                                                 existing->second);
            return;
        }
        if (_nextComponentType >= MAX_COMPONENTS)
            throw EngineError("Too many component types (MAX_COMPONENTS)");

        _componentTypes.insert({key, _nextComponentType});
        if constexpr (ComponentDescription<Component>::isDescribed)
            _componentNames.insert(
                {ComponentDescription<Component>::describe().name, _nextComponentType});
        _componentPools.insert({key, std::make_unique<ComponentPool<Component>>()});
        _nextComponentType++;

        if constexpr (ComponentDescription<Component>::isDescribed)
            _descriptors.push_back(ComponentDescription<Component>::describe());
    }

    template <typename Component>
    void registerComponent(ComponentDeserializer deserializer)
    {
        registerComponent<Component>();
        const auto &descriptor = ComponentDescription<Component>::describe();
        _deserializers.insert_or_assign(descriptor.name, std::move(deserializer));
    }

    void addComponentFromJson(Scene &scene, Entity entity, const nlohmann::json &component);
    std::function<void(Scene &, Entity)> componentInitializer(
        const nlohmann::json &component) const;

    const std::vector<ComponentDescriptor> &getDescriptors() const { return _descriptors; }

    ComponentType getComponentTypeByName(const std::string &name) const
    {
        auto iterator = _componentNames.find(name);
        if (iterator == _componentNames.end())
            throw UnknownComponentType(name);
        return iterator->second;
    }

    template <typename Component> ComponentType getComponentType() const
    {
        const std::type_index key(typeid(Component));

        if (_componentTypes.find(key) == _componentTypes.end())
            throw ComponentNotRegisteredError();

        return _componentTypes.at(key);
    }

    template <typename Component> void addComponent(Entity entity, Component component)
    {
        getComponentPool<Component>().insertComponent(entity, component);
    }

    template <typename Component> void removeComponent(Entity entity)
    {
        getComponentPool<Component>().removeComponent(entity);
    }

    template <typename Component> Component &getComponent(Entity entity)
    {
        return getComponentPool<Component>().getComponent(entity);
    }

    template <typename Component> Component &getComponent(Entity entity, std::size_t index)
    {
        return getComponentPool<Component>().getComponent(entity, index);
    }

    template <typename Component> std::size_t getComponentCount(Entity entity)
    {
        return getComponentPool<Component>().count(entity);
    }

    template <typename Component> std::vector<Component *> getComponents(Entity entity)
    {
        return getComponentPool<Component>().getComponents(entity);
    }

    void entityDestroyed(Entity entity)
    {
        for (auto const &pair : _componentPools)
            pair.second->destroyEntity(entity);
    }

  private:
    std::unordered_map<std::type_index, ComponentType> _componentTypes = {};
    std::unordered_map<std::string, ComponentType> _componentNames = {};
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> _componentPools = {};
    ComponentType _nextComponentType = {};
    std::vector<ComponentDescriptor> _descriptors = {};
    std::unordered_map<std::string, ComponentDeserializer> _deserializers = {};

    template <typename Component> ComponentPool<Component> &getComponentPool()
    {
        const std::type_index key(typeid(Component));
        auto iterator = _componentPools.find(key);

        if (iterator == _componentPools.end())
            throw ComponentNotRegisteredError();
        return static_cast<ComponentPool<Component> &>(*iterator->second);
    }
};
