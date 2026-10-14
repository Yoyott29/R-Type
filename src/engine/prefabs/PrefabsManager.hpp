/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Prefabs
*/

#ifndef PREFABS_MANAGER_HPP_
#define PREFABS_MANAGER_HPP_

#include "ComponentManager.hpp"
#include "Entity.hpp"
#include "Prefab.hpp"
#include "Scene.hpp"
#include <functional>
#include <map>

class PrefabsManager
{
  public:
    void createPrefab(std::string const &name, Prefab const &prefab) { _prefabs[name] = prefab; }
    Entity InstantiatePrefab(std::string const &name, Scene &scene)
    {
        return _prefabs[name].instantiate(scene);
    }

  protected:
  private:
    std::map<std::string, Prefab> _prefabs;
};

#endif /* !PREFABS_MANAGER_HPP_ */
