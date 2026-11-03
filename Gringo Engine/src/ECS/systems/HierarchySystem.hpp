/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** HierarchySystem
*/

#pragma once
#include "ChildOf.hpp"
#include "Position.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include <algorithm>
#include <vector>

// Runs in Update: every child moves by what its parent moved, parents before their children so
// a whole family moves together. A child can still move on its own (script, Velocity)
class HierarchySystem : public System
{
  public:
    void update(Scene &scene) override { followParents(scene); }

    // Also called once when a scene is loaded, so children are in place before the first frame
    static void followParents(Scene &scene)
    {
        std::vector<Entity> children = scene.view<ChildOf>();
        std::stable_sort(children.begin(), children.end(), [&scene](Entity left, Entity right) {
            return depth(scene, left) < depth(scene, right);
        });
        for (Entity child : children)
            follow(scene, child);
    }

  private:
    static void follow(Scene &scene, Entity child)
    {
        ChildOf &link = scene.getComponent<ChildOf>(child);
        if (link.parent == NO_PARENT || !scene.hasComponent<Position>(link.parent) ||
            !scene.hasComponent<Position>(child))
            return;
        const Position &parentPosition = scene.getComponent<Position>(link.parent);
        Position &position = scene.getComponent<Position>(child);

        // The first time, the whole parent position is added: the child's Position was relative
        const Position from = link.attached ? link.lastParentPosition : Position{0.0f, 0.0f};
        position.x += parentPosition.x - from.x;
        position.y += parentPosition.y - from.y;
        link.lastParentPosition = parentPosition;
        link.attached = true;
    }

    // Number of ancestors, capped so a ChildOf loop cannot hang the game
    static int depth(Scene &scene, Entity entity)
    {
        int depth = 0;
        while (depth < MAX_DEPTH && scene.hasComponent<ChildOf>(entity)) {
            const Entity parent = scene.getComponent<ChildOf>(entity).parent;
            if (parent == NO_PARENT || !scene.isEntityAlive(parent))
                break;
            entity = parent;
            ++depth;
        }
        return depth;
    }

    static inline const int MAX_DEPTH = 64;
};
