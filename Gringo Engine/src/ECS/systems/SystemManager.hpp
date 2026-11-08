/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SystemManager
*/

#pragma once
#include "EngineError.hpp"
#include "ScheduleGraph.hpp"
#include "System.hpp"
#include "SystemAccess.hpp"
#include "ThreadPool.hpp"
#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

// Run in this order every frame. With a renderer, the application opens the frame after
// PostUpdate, runs Render (the world, e.g. 2D sprites) then Overlay (on top of it, e.g. UI), and
// shows the frame. Without a renderer, Render and Overlay do not run
enum ScheduleLabel {
    PrepUpdate,
    StateTransition,
    FixedUpdate,
    Update,
    PostUpdate,
    Render,
    Overlay
};

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
        _graphsDirty = true;
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

    template <typename SystemType> void setAccess(SystemAccess access)
    {
        const std::type_index key(typeid(SystemType));

        if (_systems.find(key) == _systems.end()) {
            throw ComponentNotRegisteredError();
        }

        _accesses.insert_or_assign(key, std::move(access));
        _graphsDirty = true;
    }

    // nullptr means the system declared no access and runs exclusively
    const SystemAccess *findAccess(const std::type_index &system) const
    {
        auto it = _accesses.find(system);
        return it == _accesses.end() ? nullptr : &it->second;
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
        _accesses.erase(key);
        _systems.erase(it);
        _graphsDirty = true;
    }

    void entityDestroyed(Entity entity);
    void entityComponentSetChanged(Entity entity, ComponentSet entityComponentSet);
    void runSchedule(ScheduleLabel label, Scene &scene);
    // Created on first use, so a sequential app starts no thread
    ThreadPool &pool();
    bool hasPool() const { return _pool != nullptr; }

  private:
    void rebuildGraphs();

    std::unordered_map<std::type_index, ComponentSet> _componentSets = {};
    std::unordered_map<std::type_index, SystemAccess> _accesses = {};
    std::unordered_map<std::type_index, std::shared_ptr<System>> _systems{};
    std::map<ScheduleLabel, std::vector<std::shared_ptr<System>>> _systemsByLabel{};
    // Shared and immutable: a run keeps its snapshot while a rebuild swaps in new graphs
    std::map<ScheduleLabel, std::shared_ptr<const ScheduleGraph>> _graphs{};
    bool _graphsDirty = true;
    std::unique_ptr<ThreadPool> _pool;
    std::once_flag _poolOnce;
};
