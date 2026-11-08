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
#include <vector>

#define COLLIDER_DEGREES_TO_RADIANS (3.14159265f / 180.0f)
// Below this squared length a crossing of two directions counts as none: they are parallel
#define COLLIDER_PARALLEL_EPSILON 1e-6f

// Turns a direction of the collider like the renderer turns a 3D model. The renderer first turns
// a model by -90 degrees around Y: its RotationX then tilts it around the world's Z, like
// RotationZ, and RotationY turns it around Y last
Collider3DSystem::Vector Collider3DSystem::turn(Vector vector, const Transform &position,
                                                const Collider3D &collider)
{
    if (!collider.rotates)
        return vector;
    float tilt = (position.rotationX + position.rotationZ) * COLLIDER_DEGREES_TO_RADIANS;
    float yaw = -position.rotationY * COLLIDER_DEGREES_TO_RADIANS;

    // Around Z
    float x = std::cos(tilt) * vector.x - std::sin(tilt) * vector.y;
    float y = std::sin(tilt) * vector.x + std::cos(tilt) * vector.y;
    float z = vector.z;

    // Around Y
    Vector turned;
    turned.x = std::cos(yaw) * x + std::sin(yaw) * z;
    turned.y = y;
    turned.z = -std::sin(yaw) * x + std::cos(yaw) * z;
    return turned;
}

Collider3DSystem::Box Collider3DSystem::box(const Transform &position, const Collider3D &collider)
{
    Box result;
    Vector offset = turn({collider.offsetX, collider.offsetY, collider.offsetZ}, position, collider);

    result.center = {position.x + offset.x, position.y + offset.y, position.z + offset.z};
    result.axisX = turn({1.0f, 0.0f, 0.0f}, position, collider);
    result.axisY = turn({0.0f, 1.0f, 0.0f}, position, collider);
    result.axisZ = turn({0.0f, 0.0f, 1.0f}, position, collider);
    result.halfWidth = collider.width / 2.0f;
    result.halfHeight = collider.height / 2.0f;
    result.halfDepth = collider.depth / 2.0f;
    // How far the turned box goes along each world axis
    result.extent.x = std::abs(result.axisX.x) * result.halfWidth +
                      std::abs(result.axisY.x) * result.halfHeight +
                      std::abs(result.axisZ.x) * result.halfDepth;
    result.extent.y = std::abs(result.axisX.y) * result.halfWidth +
                      std::abs(result.axisY.y) * result.halfHeight +
                      std::abs(result.axisZ.y) * result.halfDepth;
    result.extent.z = std::abs(result.axisX.z) * result.halfWidth +
                      std::abs(result.axisY.z) * result.halfHeight +
                      std::abs(result.axisZ.z) * result.halfDepth;
    return result;
}

float Collider3DSystem::dot(Vector first, Vector second)
{
    return first.x * second.x + first.y * second.y + first.z * second.z;
}

Collider3DSystem::Vector Collider3DSystem::cross(Vector first, Vector second)
{
    return {first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z,
            first.x * second.y - first.y * second.x};
}

// Both boxes seen from the side, along the axis: they don't touch if the gap between their
// middles is bigger than how far they both reach towards each other
bool Collider3DSystem::apart(const Box &first, const Box &second, Vector axis)
{
    // Two parallel directions cross into nothing: nothing to test
    if (dot(axis, axis) < COLLIDER_PARALLEL_EPSILON)
        return false;
    Vector between = {second.center.x - first.center.x, second.center.y - first.center.y,
                      second.center.z - first.center.z};
    float reach = first.halfWidth * std::abs(dot(first.axisX, axis)) +
                  first.halfHeight * std::abs(dot(first.axisY, axis)) +
                  first.halfDepth * std::abs(dot(first.axisZ, axis)) +
                  second.halfWidth * std::abs(dot(second.axisX, axis)) +
                  second.halfHeight * std::abs(dot(second.axisY, axis)) +
                  second.halfDepth * std::abs(dot(second.axisZ, axis));

    return std::abs(dot(between, axis)) > reach;
}

// Two boxes don't touch if they are apart along one of the directions of either box, or one of
// the crossings of a direction of one with a direction of the other
bool Collider3DSystem::separated(const Box &first, const Box &second)
{
    std::vector<Vector> firstAxes = {first.axisX, first.axisY, first.axisZ};
    std::vector<Vector> secondAxes = {second.axisX, second.axisY, second.axisZ};

    for (Vector axis : firstAxes) {
        if (apart(first, second, axis))
            return true;
    }
    for (Vector axis : secondAxes) {
        if (apart(first, second, axis))
            return true;
    }
    for (Vector firstAxis : firstAxes) {
        for (Vector secondAxis : secondAxes) {
            if (apart(first, second, cross(firstAxis, secondAxis)))
                return true;
        }
    }
    return false;
}

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
            Box first = box(position, collider);
            Box second = box(otherPosition, otherCollider);
            Vector delta = {second.center.x - first.center.x, second.center.y - first.center.y,
                            second.center.z - first.center.z};
            float overlapX = first.extent.x + second.extent.x - std::abs(delta.x);
            float overlapY = first.extent.y + second.extent.y - std::abs(delta.y);
            float overlapZ = first.extent.z + second.extent.z - std::abs(delta.z);
            if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f)
                continue;
            // The boxes lined up with the world touch: turned ones may still miss each other
            if ((collider.rotates || otherCollider.rotates) && separated(first, second))
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
            const bool step = climbs(scene, entity, first, second) ||
                              climbs(scene, other, second, first);
            if (step) {
                overlapX = std::numeric_limits<float>::max();
                overlapZ = std::numeric_limits<float>::max();
            }
            resolve(scene, entity, other, position, otherPosition, overlapX, overlapY, overlapZ,
                    delta);
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

bool Collider3DSystem::climbs(Scene &scene, Entity body, const Box &bodyBox, const Box &stepBox)
{
    if (!dynamic(scene, body))
        return false;
    const float feet = bodyBox.center.y - bodyBox.extent.y;
    const float top = stepBox.center.y + stepBox.extent.y;
    return top > feet && top - feet <= scene.getComponent<RigidBody3D>(body).stepHeight;
}

void Collider3DSystem::resolve(Scene &scene, Entity first, Entity second, Transform &firstPosition,
                               Transform &secondPosition, float overlapX, float overlapY,
                               float overlapZ, Vector delta)
{
    const float inverseFirst = dynamic(scene, first) ? 1.0f / scene.getComponent<RigidBody3D>(first).mass : 0.0f;
    const float inverseSecond = dynamic(scene, second) ? 1.0f / scene.getComponent<RigidBody3D>(second).mass : 0.0f;
    const float total = inverseFirst + inverseSecond;
    if (total == 0.0f)
        return;
    const float depth = std::min({overlapX, overlapY, overlapZ});
    const bool xAxis = depth == overlapX;
    const bool yAxis = depth == overlapY;
    const float along = xAxis ? delta.x : yAxis ? delta.y : delta.z;
    const float normal = along < 0.0f ? -1.0f : 1.0f;
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
