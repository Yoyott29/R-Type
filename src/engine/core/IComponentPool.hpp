/*
** EPITECH PROJECT, 2026
** G-ING-500-LIL-5-1-rtype-4
** File description:
** ComponentPool
*/

#pragma once
#include "Entity.hpp"

class IComponentPool {
    public:
        virtual ~IComponentPool() = default;
        virtual void destroyEntity(Entity entity) = 0;
};
