/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SystemAccess
*/

#pragma once
#include "ComponentManager.hpp"
#include "Entity.hpp"
#include <set>
#include <typeindex>

// What a system reads and writes. Two systems whose accesses do not conflict can run in parallel
class SystemAccess
{
  public:
    explicit SystemAccess(const ComponentManager &components) : _components(&components) {}

    template <typename Component> SystemAccess &read()
    {
        _componentReads.set(_components->getComponentType<Component>());
        return *this;
    }

    // Writing a component also reads it
    template <typename Component> SystemAccess &write()
    {
        read<Component>();
        _componentWrites.set(_components->getComponentType<Component>());
        return *this;
    }

    template <typename Resource> SystemAccess &readResource()
    {
        _resourceReads.insert(std::type_index(typeid(Resource)));
        return *this;
    }

    // Writing a resource also reads it
    template <typename Resource> SystemAccess &writeResource()
    {
        readResource<Resource>();
        _resourceWrites.insert(std::type_index(typeid(Resource)));
        return *this;
    }

    const ComponentSet &componentReads() const { return _componentReads; }
    const ComponentSet &componentWrites() const { return _componentWrites; }
    const std::set<std::type_index> &resourceReads() const { return _resourceReads; }
    const std::set<std::type_index> &resourceWrites() const { return _resourceWrites; }

  private:
    const ComponentManager *_components;
    ComponentSet _componentReads = {};
    ComponentSet _componentWrites = {};
    std::set<std::type_index> _resourceReads = {};
    std::set<std::type_index> _resourceWrites = {};
};
