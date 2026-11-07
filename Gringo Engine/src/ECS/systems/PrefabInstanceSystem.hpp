/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** PrefabInstanceSystem
*/

#pragma once
#include "EngineError.hpp"
#include "PrefabInstance.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Transform.hpp"
#include <string>
#include <vector>

// Once, when the game starts: creates the prefab of every PrefabInstance of the scene. The entity's
// Transform places the copy: positions and rotations add up, scales multiply
class PrefabInstanceSystem : public System
{
  public:
    void Start(Scene &scene) override
    {
        // Copied: a new entity can join _entities if its prefab holds a PrefabInstance
        const std::vector<Entity> instances(_entities.begin(), _entities.end());
        for (Entity entity : instances)
            spawn(scene, entity);
    }

    void update(Scene &) override {}

  private:
    static void spawn(Scene &scene, Entity entity)
    {
        const std::string name = scene.getComponent<PrefabInstance>(entity).prefab;
        if (name.empty())
            return;
        if (!scene.hasResource<PrefabsManager>() ||
            !scene.getResource<PrefabsManager>().hasPrefab(name))
            throw EngineError("Unknown prefab: " + name);
        const Entity copy = scene.getResource<PrefabsManager>().InstantiatePrefab(name, scene);
        if (!scene.hasComponent<Transform>(entity))
            return;
        if (!scene.hasComponent<Transform>(copy))
            scene.addComponent(copy, Transform{});
        place(scene.getComponent<Transform>(copy), scene.getComponent<Transform>(entity));
    }

    static void place(Transform &copy, const Transform &holder)
    {
        copy.x += holder.x;
        copy.y += holder.y;
        copy.z += holder.z;
        copy.scaleX *= holder.scaleX;
        copy.scaleY *= holder.scaleY;
        copy.scaleZ *= holder.scaleZ;
        copy.rotationX += holder.rotationX;
        copy.rotationY += holder.rotationY;
        copy.rotationZ += holder.rotationZ;
    }
};
