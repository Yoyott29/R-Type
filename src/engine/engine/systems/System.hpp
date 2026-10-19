/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** System
*/

#pragma once
#include "Entity.hpp"
#include <set>

class Scene;
class System
{
  public:
    virtual ~System() = default;
    virtual void update(Scene &scene) = 0;
    std::set<Entity> _entities;
};
