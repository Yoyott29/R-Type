/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** System
*/

#pragma once
#include "Entity.hpp"
#include <set>

class System
{
  public:
    virtual ~System() = default;
    std::set<Entity> _entities;
};
