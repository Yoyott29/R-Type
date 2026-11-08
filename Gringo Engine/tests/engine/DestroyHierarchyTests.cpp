/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** DestroyHierarchyTests
*/

#include "Check.hpp"
#include "ChildOf.hpp"
#include "Scene.hpp"

namespace
{
Entity addChild(Scene &scene, Entity parent)
{
    const Entity child = scene.createEntity();
    ChildOf link;
    link.parent = parent;
    scene.addComponent(child, link);
    return child;
}
} // namespace

TEST_CASE(destroyEntityTakesItsDescendants)
{
    Scene scene;
    scene.registerComponent<ChildOf>();
    const Entity parent = scene.createEntity();
    const Entity other = scene.createEntity();
    const Entity child = addChild(scene, parent);
    const Entity grandChild = addChild(scene, child);
    const Entity otherChild = addChild(scene, other);

    scene.destroyEntity(parent);
    CHECK(!scene.isEntityAlive(parent));
    CHECK(!scene.isEntityAlive(child));
    CHECK(!scene.isEntityAlive(grandChild));
    CHECK(scene.isEntityAlive(other));
    CHECK(scene.isEntityAlive(otherChild));
    CHECK(scene.getComponent<ChildOf>(otherChild).parent == other);
}

TEST_CASE(destroyEntityWithoutChildOfRegistered)
{
    Scene scene;
    const Entity entity = scene.createEntity();
    scene.destroyEntity(entity);
    CHECK(!scene.isEntityAlive(entity));
}
