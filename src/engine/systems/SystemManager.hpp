/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SystemManager
*/

#pragma once
#include "System.hpp"
#include <iostream>
#include <memory>
#include <typeindex>
#include <unordered_map>

class SystemManager
{
  public:
    template <typename SystemType> std::shared_ptr<SystemType> registerSystem()
    {
        const std::type_index key(typeid(SystemType));

        if (_systems.find(key) != _systems.end()) {
            std::cout << "System registered more than once." << "\n";
            return nullptr;
        }

        auto system = std::make_shared<SystemType>();
        _systems.insert({key, system});
        return system;
    }

    template <typename SystemType> void setComponentSet(ComponentSet componentSet)
    {
        const std::type_index key(typeid(SystemType));

        if (_systems.find(key) == _systems.end()) {
            std::cout << "System used before registered." << "\n";
            return;
        }

        _componentSets.insert_or_assign(key, componentSet);
    }

    void entityDestroyed(Entity entity);
    void entityComponentSetChanged(Entity entity, ComponentSet entityComponentSet);

  private:
    std::unordered_map<std::type_index, ComponentSet> _componentSets = {};
    std::unordered_map<std::type_index, std::shared_ptr<System>> _systems{};
};
