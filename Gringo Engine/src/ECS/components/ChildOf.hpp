/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ChildOf
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include "Entity.hpp"
#include "Transform.hpp"

// parent of a ChildOf whose parent is not known (yet)
const Entity NO_PARENT = MAX_ENTITIES;

// Makes an entity the child of another one: its Transform is relative to the parent, it follows
// the parent's moves (HierarchySystem) and it is destroyed with it (Scene::destroyEntity)
struct ChildOf {
    // The parent's id in the editor (.gringo file), resolved into parent when the scene loads
    int parentId = -1;
    Entity parent = NO_PARENT;
    // Where the parent was the last time the child followed it
    Transform lastParentPosition{0.0f, 0.0f};
    // False until the child's relative Transform has been turned into a world position
    bool attached = false;
};

template <> struct ComponentDescription<ChildOf> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"ChildOf",
                {
                    {"Parent", valueType::Int, "-1"},
                },
                {63, 58, 53, 255},
                ""};
    }
};
