/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Collider2DSystem
*/

#include "Collider2DSystem.hpp"
#include "Sprite.hpp"
#include "Type.hpp"
#include <set>
#include <variant>

static bool isPlayer(Scene &scene, Entity entity)
{
    return scene.getComponentCount<Type>(entity) > 0 &&
           scene.getComponent<Type>(entity).type == EntType::PLAYER;
}

void Collider2DSystem::update(Scene &scene)
{
    std::set<Entity> checkedEntities;

    for (Entity entity : _entities) {
        Position &position = scene.getComponent<Position>(entity);
        Collider2D &collider = scene.getComponent<Collider2D>(entity);
        bool entityIsPlayer = isPlayer(scene, entity);

        for (Entity otherEntity : checkedEntities) {
            if (entityIsPlayer && isPlayer(scene, otherEntity))
                continue;
            Position &otherPosition = scene.getComponent<Position>(otherEntity);
            Collider2D &otherCollider = scene.getComponent<Collider2D>(otherEntity);

            bool isColliding = checkCollisions(position, collider, otherPosition, otherCollider);

            if (isColliding) {
                if (collider.onCollision)
                    collider.onCollision(entity, otherEntity);
                if (otherCollider.onCollision)
                    otherCollider.onCollision(otherEntity, entity);
            }
        }

        checkedEntities.insert(entity);
    }
}

bool Collider2DSystem::checkCollisions(const Position &positionA, const Collider2D &colliderA,
                                       const Position &positionB, const Collider2D &colliderB)
{
    return std::visit(
        [&](const auto &shapeA, const auto &shapeB) {
            return checkOverlaps(positionA, shapeA, positionB, shapeB);
        },
        colliderA.shape, colliderB.shape);
}

// Box vs Box
bool Collider2DSystem::checkOverlaps(const Position &positionA, const BoxShape &colliderA,
                                     const Position &positionB, const BoxShape &colliderB)
{
    return std::abs(positionA.x - positionB.x) < (colliderA.width + colliderB.width) / 2.0f &&
           std::abs(positionA.y - positionB.y) < (colliderA.height + colliderB.height) / 2.0f;
}

// Circle vs Circle
bool Collider2DSystem::checkOverlaps(const Position &positionA, const CircleShape &colliderA,
                                     const Position &positionB, const CircleShape &colliderB)
{
    float dx = positionA.x - positionB.x;
    float dy = positionA.y - positionB.y;
    float radiusSum = colliderA.radius + colliderB.radius;

    // Compare squared distances to avoid a sqrt
    return dx * dx + dy * dy < radiusSum * radiusSum;
}

// Box vs Circle
bool Collider2DSystem::checkOverlaps(const Position &positionA, const BoxShape &colliderA,
                                     const Position &positionB, const CircleShape &colliderB)
{
    float halfWidth = colliderA.width / 2.0f;
    float halfHeight = colliderA.height / 2.0f;

    // Closest point on the box to the circle's center
    float closestX = std::clamp(positionB.x, positionA.x - halfWidth, positionA.x + halfWidth);
    float closestY = std::clamp(positionB.y, positionA.y - halfHeight, positionA.y + halfHeight);

    float dx = positionB.x - closestX;
    float dy = positionB.y - closestY;

    return dx * dx + dy * dy < colliderB.radius * colliderB.radius;
}

// Circle vs Box
bool Collider2DSystem::checkOverlaps(const Position &positionA, const CircleShape &colliderA,
                                     const Position &positionB, const BoxShape &colliderB)
{
    return checkOverlaps(positionB, colliderB, positionA, colliderA);
}
