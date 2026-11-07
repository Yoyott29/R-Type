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
#include <limits>
#include <utility>

void Collider3DSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        if (dynamic(scene, entity))
            scene.getComponent<RigidBody3D>(entity).grounded = false;
    }

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
                                   std::abs(position.x + collider.offsetX - otherPosition.x -
                                            otherCollider.offsetX);
            const float overlapY = (collider.height + otherCollider.height) / 2.0f -
                                   std::abs(position.y + collider.offsetY - otherPosition.y -
                                            otherCollider.offsetY);
            const float overlapZ = (collider.depth + otherCollider.depth) / 2.0f -
                                   std::abs(position.z + collider.offsetZ - otherPosition.z -
                                            otherCollider.offsetZ);
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
            // On a low step: pushed up onto it, never sideways
            const bool step =
                climbs(scene, entity, position, collider, otherPosition, otherCollider) ||
                climbs(scene, other, otherPosition, otherCollider, position, collider);
            const float sideways = std::numeric_limits<float>::max();
            resolve(scene, entity, other, position, otherPosition, collider, otherCollider,
                    step ? sideways : overlapX, overlapY, step ? sideways : overlapZ);
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

bool Collider3DSystem::climbs(Scene &scene, Entity body, const Transform &bodyPosition,
                              const Collider3D &bodyCollider, const Transform &stepPosition,
                              const Collider3D &stepCollider)
{
    if (!dynamic(scene, body))
        return false;
    const float feet = bodyPosition.y + bodyCollider.offsetY - bodyCollider.height / 2.0f;
    const float top = stepPosition.y + stepCollider.offsetY + stepCollider.height / 2.0f;
    return top > feet && top - feet <= scene.getComponent<RigidBody3D>(body).stepHeight;
}

void Collider3DSystem::resolve(Scene &scene, Entity first, Entity second, Transform &firstPosition,
                               Transform &secondPosition, const Collider3D &firstCollider,
                               const Collider3D &secondCollider, float overlapX, float overlapY,
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
    const float deltaX = secondPosition.x + secondCollider.offsetX - firstPosition.x -
                         firstCollider.offsetX;
    const float deltaY = secondPosition.y + secondCollider.offsetY - firstPosition.y -
                         firstCollider.offsetY;
    const float deltaZ = secondPosition.z + secondCollider.offsetZ - firstPosition.z -
                         firstCollider.offsetZ;
    const float delta = xAxis ? deltaX : yAxis ? deltaY : deltaZ;
    const float normal = delta < 0.0f ? -1.0f : 1.0f;
    if (yAxis) {
        if (normal < 0.0f && dynamic(scene, first))
            scene.getComponent<RigidBody3D>(first).grounded = true;
        if (normal > 0.0f && dynamic(scene, second))
            scene.getComponent<RigidBody3D>(second).grounded = true;
    }
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
