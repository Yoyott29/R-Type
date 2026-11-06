/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Collider3DSystem
*/

#include "Collider3DSystem.hpp"
#include "RigidBody3D.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

void Collider3DSystem::update(Scene &scene)
{
    std::set<Entity> checked;
    std::set<std::pair<Entity, Entity>> solidContacts;
    for (Entity entity : _entities) {
        Transform &position = scene.getComponent<Transform>(entity);
        Collider3D &collider = scene.getComponent<Collider3D>(entity);
        for (Entity other : checked) {
            Transform &otherPosition = scene.getComponent<Transform>(other);
            Collider3D &otherCollider = scene.getComponent<Collider3D>(other);
            if ((collider.mask & otherCollider.layer) == 0 &&
                (otherCollider.mask & collider.layer) == 0)
                continue;
            const float overlapX = (collider.width + otherCollider.width) / 2.0f -
                                   std::abs(position.x - otherPosition.x);
            const float overlapY = (collider.height + otherCollider.height) / 2.0f -
                                   std::abs(position.y - otherPosition.y);
            const float overlapZ = (collider.depth + otherCollider.depth) / 2.0f -
                                   std::abs(position.z - otherPosition.z);
            if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f)
                continue;
            const bool trigger = collider.isTrigger || otherCollider.isTrigger;
            if (trigger) {
                if (collider.onCollision && (collider.mask & otherCollider.layer) != 0)
                    collider.onCollision(entity, other);
                if (otherCollider.onCollision && (otherCollider.mask & collider.layer) != 0)
                    otherCollider.onCollision(other, entity);
                continue;
            }
            const auto pair = std::minmax(entity, other);
            solidContacts.insert(pair);
            resolve(scene, entity, other, position, otherPosition, overlapX, overlapY, overlapZ);
        }
        checked.insert(entity);
    }
    _solidContacts = std::move(solidContacts);
}

bool Collider3DSystem::dynamic(Scene &scene, Entity entity)
{
    return scene.hasComponent<RigidBody3D>(entity) && scene.hasComponent<Velocity3D>(entity) &&
           !scene.getComponent<RigidBody3D>(entity).isStatic;
}

void Collider3DSystem::resolve(Scene &scene, Entity first, Entity second, Transform &firstPosition,
                               Transform &secondPosition, float overlapX, float overlapY,
                               float overlapZ)
{
    const float inverseFirst = dynamic(scene, first) ? 1.0f / scene.getComponent<RigidBody3D>(first).mass : 0.0f;
    const float inverseSecond = dynamic(scene, second) ? 1.0f / scene.getComponent<RigidBody3D>(second).mass : 0.0f;
    const float total = inverseFirst + inverseSecond;
    if (total == 0.0f)
        return;
    const float depth = std::min({overlapX, overlapY, overlapZ});
    const bool xAxis = depth == overlapX;
    const bool yAxis = depth == overlapY;
    const float delta = xAxis ? secondPosition.x - firstPosition.x :
                        yAxis ? secondPosition.y - firstPosition.y :
                                secondPosition.z - firstPosition.z;
    const float normal = delta < 0.0f ? -1.0f : 1.0f;
    const float pushFirst = depth * inverseFirst / total;
    const float pushSecond = depth * inverseSecond / total;
    if (xAxis) { firstPosition.x -= normal * pushFirst; secondPosition.x += normal * pushSecond; }
    else if (yAxis) { firstPosition.y -= normal * pushFirst; secondPosition.y += normal * pushSecond; }
    else { firstPosition.z -= normal * pushFirst; secondPosition.z += normal * pushSecond; }

    const Velocity3D firstVelocity =
        scene.hasComponent<Velocity3D>(first) ? scene.getComponent<Velocity3D>(first)
                                               : Velocity3D{};
    const Velocity3D secondVelocity =
        scene.hasComponent<Velocity3D>(second) ? scene.getComponent<Velocity3D>(second)
                                                : Velocity3D{};
    const float relativeVelocity =
        (secondVelocity.dx - firstVelocity.dx) * (xAxis ? normal : 0.0f) +
        (secondVelocity.dy - firstVelocity.dy) * (yAxis ? normal : 0.0f) +
        (secondVelocity.dz - firstVelocity.dz) * (!xAxis && !yAxis ? normal : 0.0f);
    if (relativeVelocity >= 0.0f)
        return;

    const float impulse = -relativeVelocity / total;
    if (inverseFirst != 0.0f) {
        Velocity3D &velocity = scene.getComponent<Velocity3D>(first);
        if (xAxis)
            velocity.dx -= normal * impulse * inverseFirst;
        else if (yAxis)
            velocity.dy -= normal * impulse * inverseFirst;
        else
            velocity.dz -= normal * impulse * inverseFirst;
    }
    if (inverseSecond != 0.0f) {
        Velocity3D &velocity = scene.getComponent<Velocity3D>(second);
        if (xAxis)
            velocity.dx += normal * impulse * inverseSecond;
        else if (yAxis)
            velocity.dy += normal * impulse * inverseSecond;
        else
            velocity.dz += normal * impulse * inverseSecond;
    }
}
