/*
** EPITECH PROJECT, 2026
** G-ING-500-LIL-5-1-rtype-4
** File description:
** ComponentPool
*/

#pragma once
#include "EngineError.hpp"
#include "Entity.hpp"
#include "IComponentPool.hpp"
#include <array>
#include <vector>

template <typename Component> class ComponentPool : public IComponentPool
{
  public:
    ComponentPool() { _entityToPackedIndex.fill(NULL_INDEX); }

    void insertComponent(Entity entity, Component component)
    {
        if (has(entity)) {
            std::cout << "Tried to add a component on the same entity twice" << std::endl;
            return;
        }

        _entityToPackedIndex[entity] = _packedComponents.size();
        _packedComponents.push_back(component);
        _packedIndexToEntity.push_back(entity);
    }

    void removeComponent(Entity entity)
    {
        if (!has(entity)) {
            std::cout << "Tried to remove a component from an entity that didn't have it"
                      << std::endl;
            return;
        }

        const std::size_t indexToRemove = _entityToPackedIndex[entity];
        const std::size_t lastIndex = _packedComponents.size() - 1;
        const Entity entityAtLastIndex = _packedIndexToEntity[lastIndex];

        _packedComponents[indexToRemove] = _packedComponents[lastIndex];
        _packedIndexToEntity[indexToRemove] = entityAtLastIndex;
        _entityToPackedIndex[entityAtLastIndex] = indexToRemove;

        _packedComponents.pop_back();
        _packedIndexToEntity.pop_back();
        _entityToPackedIndex[entity] = NULL_INDEX;
    }

    bool has(Entity entity) const
    {
        return entity < _entityToPackedIndex.size() && _entityToPackedIndex[entity] != NULL_INDEX;
    }

    Component &getComponent(Entity entity)
    {
        if (!has(entity))
            throw MissingComponentError(entity);

        return _packedComponents[_entityToPackedIndex[entity]];
    }

    void destroyEntity(Entity entity) override
    {
        if (has(entity))
            removeComponent(entity);
    }

  private:
    std::array<std::size_t, MAX_ENTITIES> _entityToPackedIndex;
    std::vector<Component> _packedComponents;
    std::vector<Entity> _packedIndexToEntity;
};
