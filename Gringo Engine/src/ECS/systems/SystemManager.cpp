/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SystemManager
*/

#include "SystemManager.hpp"
#include "ScheduleRunner.hpp"

void SystemManager::entityDestroyed(Entity entity)
{
    for (auto const &pair : _systems)
        pair.second->_entities.erase(entity);
}

void SystemManager::entityComponentSetChanged(Entity entity, ComponentSet componentSet)
{
    for (auto const &pair : _systems) {
        auto requiredIterator = _componentSets.find(pair.first);
        if (requiredIterator == _componentSets.end())
            continue;

        ComponentSet const &requiredSet = requiredIterator->second;
        if ((componentSet & requiredSet) == requiredSet)
            pair.second->_entities.insert(entity);
        else
            pair.second->_entities.erase(entity);
    }
}

void SystemManager::rebuildGraphs()
{
    _graphs.clear();
    for (auto const &[label, systems] : _systemsByLabel) {
        std::vector<const SystemAccess *> accesses;
        accesses.reserve(systems.size());
        for (auto const &system : systems) {
            // Named first: typeid(*system) warns about the side effects of operator*
            const System &instance = *system;
            accesses.push_back(findAccess(std::type_index(typeid(instance))));
        }
        _graphs.emplace(label, std::make_shared<const ScheduleGraph>(
                                   buildScheduleGraph(systems, accesses)));
    }
}

void SystemManager::runSchedule(ScheduleLabel label, Scene &scene)
{
    if (_graphsDirty) {
        rebuildGraphs();
        _graphsDirty = false;
    }
    auto it = _graphs.find(label);
    if (it == _graphs.end())
        return;
    // Snapshot so a system can register or remove systems during the run: it takes effect next
    // call, and the snapshot keeps the removed systems alive until the run ends
    const std::shared_ptr<const ScheduleGraph> snapshot = it->second;
    const ScheduleGraph &graph = *snapshot;
    if (!graph.hasParallelism) {
        for (auto const &system : graph.systems) {
            system->runStart(scene);
            system->update(scene);
        }
        return;
    }
    if (!_pool)
        _pool = std::make_unique<ThreadPool>();
    runParallel(graph, scene, *_pool);
}
