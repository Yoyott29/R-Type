/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SystemManager
*/

#pragma once
#include "EngineError.hpp"
#include "System.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

enum ScheduleLabel { PrepUpdate, StateTransition, FixedUpdate, Update, PostUpdate };

class SystemManager
{
  public:
    template <typename SystemType>
    std::shared_ptr<SystemType> registerSystem(ScheduleLabel label = Update)
    {
        const std::type_index key(typeid(SystemType));

        if (_systems.find(key) != _systems.end()) {
            throw RegisterError();
        }

        auto system = std::make_shared<SystemType>();
        _systems.insert({key, system});
        _systemsByLabel[label].push_back(system);
        return system;
    }

    template <typename SystemType> void setComponentSet(ComponentSet componentSet)
    {
        const std::type_index key(typeid(SystemType));

        if (_systems.find(key) == _systems.end()) {
            throw ComponentNotRegisteredError();
        }

        _componentSets.insert_or_assign(key, componentSet);
    }

    template <typename SystemType> void removeSystem()
    {
        const std::type_index key(typeid(SystemType));

        auto it = _systems.find(key);
        if (it == _systems.end()) {
            throw ComponentNotRegisteredError();
        }

        for (auto &pair : _systemsByLabel) {
            auto &systems = pair.second;
            systems.erase(std::remove(systems.begin(), systems.end(), it->second), systems.end());
        }
        _componentSets.erase(key);
        _systems.erase(it);
    }

    void entityDestroyed(Entity entity);
    void entityComponentSetChanged(Entity entity, ComponentSet entityComponentSet);
    void runSchedule(ScheduleLabel label, Scene &scene);

  private:
    std::unordered_map<std::type_index, ComponentSet> _componentSets = {};
    std::unordered_map<std::type_index, std::shared_ptr<System>> _systems{};
    std::map<ScheduleLabel, std::vector<std::shared_ptr<System>>> _systemsByLabel{};
};
