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
    virtual void Start(Scene &) {}
    virtual void update(Scene &scene) = 0;
    void runStart(Scene &scene)
    {
        if (_started)
            return;
        _started = true;
        Start(scene);
    }
    std::set<Entity> _entities;

  private:
    bool _started = false;
};
