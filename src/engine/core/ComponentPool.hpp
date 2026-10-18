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
#include <algorithm>
#include <array>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

template <typename Component> struct AllowMultiple : std::false_type {
};

template <typename Component> class ComponentPool : public IComponentPool
{
  public:
    void insertComponent(Entity entity, Component component)
    {
        if (entity >= MAX_ENTITIES)
            throw OutOfRangeEntityError();
        if (!AllowMultiple<Component>::value && has(entity)) {
            throw MultipleComponentDefinition("entity " + std::to_string(entity));
        }

        _entityToPackedIndices[entity].push_back(_packedComponents.size());
        _packedComponents.push_back(component);
        _packedIndexToEntity.push_back(entity);
    }

    void removeComponent(Entity entity)
    {
        if (!has(entity)) {
            throw MissingComponentError(entity);
        }

        std::vector<std::size_t> indices = std::move(_entityToPackedIndices[entity]);
        _entityToPackedIndices[entity].clear();

        std::sort(indices.begin(), indices.end(), std::greater<std::size_t>());
        for (std::size_t index : indices)
            erasePacked(index);
    }

    bool has(Entity entity) const { return count(entity) > 0; }

    std::size_t count(Entity entity) const
    {
        return entity < MAX_ENTITIES ? _entityToPackedIndices[entity].size() : 0;
    }

    Component &getComponent(Entity entity, std::size_t index = 0)
    {
        if (index >= count(entity))
            throw MissingComponentError(entity);

        return _packedComponents[_entityToPackedIndices[entity][index]];
    }

    void destroyEntity(Entity entity) override
    {
        if (has(entity))
            removeComponent(entity);
    }

  private:
    void erasePacked(std::size_t index)
    {
        const std::size_t lastIndex = _packedComponents.size() - 1;

        if (index != lastIndex) {
            const Entity entityAtLastIndex = _packedIndexToEntity[lastIndex];

            _packedComponents[index] = std::move(_packedComponents[lastIndex]);
            _packedIndexToEntity[index] = entityAtLastIndex;

            for (std::size_t &ownerIndex : _entityToPackedIndices[entityAtLastIndex]) {
                if (ownerIndex == lastIndex) {
                    ownerIndex = index;
                    break;
                }
            }
        }
        _packedComponents.pop_back();
        _packedIndexToEntity.pop_back();
    }

    std::array<std::vector<std::size_t>, MAX_ENTITIES> _entityToPackedIndices;
    std::vector<Component> _packedComponents;
    std::vector<Entity> _packedIndexToEntity;
};
