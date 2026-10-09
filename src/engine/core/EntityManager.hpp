/*
** EPITECH PROJECT, 2026
** G-ING-500-LIL-5-1-rtype-4
** File description:
** EntityManager
*/

#pragma once
#include <queue>
#include <array>
#include <iostream>
#include "Entity.hpp"

class EntityManager {
    public:
        EntityManager();

        Entity createEntity();
        void destroyEntity(Entity entity);
        void setupComponentSet(Entity entity, ComponentSet componentSet);
        ComponentSet getComponentSet(Entity entity) const;
        bool isAlive(Entity entity) const;

    private:
        std::queue<Entity> _availableEntities = {};
        std::array<ComponentSet, MAX_ENTITIES> _componentSets = {};
        std::uint32_t _livingEntityCount = {};
        std::bitset<MAX_ENTITIES> _alive = {};
};
