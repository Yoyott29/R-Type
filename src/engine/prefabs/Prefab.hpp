/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Prefab
*/

#ifndef PREFAB_HPP_
#define PREFAB_HPP_

#include "Entity.hpp"
#include "Scene.hpp"
#include <functional>

class Prefab
{
  public:
    template <typename Component> Prefab &addComponent(Component component)
    {
        _components.push_back(
            [component](Scene &scene, Entity entity) { scene.addComponent(entity, component); });
        return *this;
    }

    Entity instantiate(Scene &scene) const
    {
        Entity entity = scene.createEntity();
        for (auto const &component : _components)
            component(scene, entity);
        return entity;
    }

  private:
    std::vector<std::function<void(Scene &, Entity)>> _components;
};

#endif /* !PREFAB_HPP_ */
