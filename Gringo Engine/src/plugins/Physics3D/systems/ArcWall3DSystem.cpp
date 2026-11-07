/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ArcWall3DSystem
*/

#include "ArcWall3DSystem.hpp"
#include "RigidBody3D.hpp"
#include "Velocity3D.hpp"
#include <algorithm>
#include <cmath>

void ArcWall3DSystem::update(Scene &scene)
{
    const std::vector<Entity> bodies = scene.view<Collider3D>();
    for (Entity wallEntity : _entities) {
        const Transform &centre = scene.getComponent<Transform>(wallEntity);
        const ArcWall3D &wall = scene.getComponent<ArcWall3D>(wallEntity);
        for (Entity body : bodies) {
            if (!scene.hasComponent<RigidBody3D>(body) || !scene.hasComponent<Velocity3D>(body) ||
                !scene.hasComponent<Transform>(body) ||
                scene.getComponent<RigidBody3D>(body).isStatic ||
                scene.getComponent<Collider3D>(body).isTrigger)
                continue;
            push(scene, body, centre, wall);
        }
    }
}

float ArcWall3DSystem::wrap(float degrees)
{
    const float wrapped = std::fmod(degrees, 360.0f);
    return wrapped < 0.0f ? wrapped + 360.0f : wrapped;
}

bool ArcWall3DSystem::inSector(const ArcWall3D &wall, float dx, float dz)
{
    const float span = wrap(wall.endAngle - wall.startAngle);
    if (span == 0.0f)
        return true;
    return wrap(std::atan2(dz, dx) * DEGREES - wall.startAngle) <= span;
}

void ArcWall3DSystem::push(Scene &scene, Entity body, const Transform &centre,
                           const ArcWall3D &wall)
{
    Transform &position = scene.getComponent<Transform>(body);
    const Collider3D &collider = scene.getComponent<Collider3D>(body);
    const float x = position.x + collider.offsetX;
    const float y = position.y + collider.offsetY;
    const float z = position.z + collider.offsetZ;
    if (std::abs(y - centre.y) >= (collider.height + wall.height) / 2.0f)
        return;
    const float dx = x - centre.x;
    const float dz = z - centre.z;
    const float distance = std::hypot(dx, dz);
    if (distance == 0.0f || !inSector(wall, dx, dz))
        return;

    const float halfWidth = collider.width / 2.0f;
    const float halfDepth = collider.depth / 2.0f;
    float depth = 0.0f;
    if (wall.keepInside) {
        // The corner of the box the farthest from the centre must stay inside the circle
        depth = std::hypot(std::abs(dx) + halfWidth, std::abs(dz) + halfDepth) - wall.radius;
    } else {
        // The point of the box the nearest to the centre must stay outside of it
        const float nearX = std::clamp(0.0f, dx - halfWidth, dx + halfWidth);
        const float nearZ = std::clamp(0.0f, dz - halfDepth, dz + halfDepth);
        depth = wall.radius - std::hypot(nearX, nearZ);
    }
    if (depth <= 0.0f)
        return;

    // Out of the wall along the radius: toward the centre for keepInside, away from it otherwise
    const float side = wall.keepInside ? -1.0f : 1.0f;
    const float normalX = side * dx / distance;
    const float normalZ = side * dz / distance;
    position.x += normalX * depth;
    position.z += normalZ * depth;

    Velocity3D &velocity = scene.getComponent<Velocity3D>(body);
    const float towardWall = velocity.dx * normalX + velocity.dz * normalZ;
    if (towardWall < 0.0f) {
        velocity.dx -= normalX * towardWall;
        velocity.dz -= normalZ * towardWall;
    }
}
