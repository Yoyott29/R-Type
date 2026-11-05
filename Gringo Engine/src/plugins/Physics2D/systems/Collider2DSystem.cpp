/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Collider2DSystem
*/

#include "Collider2DSystem.hpp"
#include <cmath>
#include <set>
#include <variant>

void Collider2DSystem::update(Scene &scene)
{
    std::set<Entity> checkedEntities;
    std::set<std::pair<Entity, Entity>> solidContacts;

    for (Entity entity : _entities) {
        Transform &position = scene.getComponent<Transform>(entity);
        Collider2D &collider = scene.getComponent<Collider2D>(entity);

        for (Entity otherEntity : checkedEntities) {
            Collider2D &otherCollider = scene.getComponent<Collider2D>(otherEntity);
            bool entityWants = (collider.mask & otherCollider.layer) != 0;
            bool otherWants = (otherCollider.mask & collider.layer) != 0;
            if (!entityWants && !otherWants)
                continue;
            Transform &otherPosition = scene.getComponent<Transform>(otherEntity);

            const std::optional<Contact> contact =
                checkCollisions(position, collider, otherPosition, otherCollider);
            if (!contact)
                continue;

            // one trigger is enough for the pair to be a trigger contact
            if (collider.isTrigger || otherCollider.isTrigger) {
                handleTrigger(entity, collider, entityWants, otherEntity, otherCollider,
                              otherWants);
                continue;
            }
            std::pair<Entity, Entity> pair = std::minmax(entity, otherEntity);
            solidContacts.insert(pair);
            if (!_solidContacts.contains(pair))
                handleSolidEnter(scene, entity, otherEntity);
            resolveSolid(scene, entity, position, otherEntity, otherPosition, *contact);
        }

        checkedEntities.insert(entity);
    }
    _solidContacts = std::move(solidContacts);
}

void Collider2DSystem::handleTrigger(Entity entity, const Collider2D &collider, bool entityWants,
                                     Entity otherEntity, const Collider2D &otherCollider,
                                     bool otherWants)
{
    if (entityWants && collider.onCollision)
        collider.onCollision(entity, otherEntity);
    if (otherWants && otherCollider.onCollision)
        otherCollider.onCollision(otherEntity, entity);
}

bool Collider2DSystem::isDynamic(Scene &scene, Entity entity)
{
    return scene.hasComponent<RigidBody2D>(entity) && scene.hasComponent<Velocity>(entity) &&
           !scene.getComponent<RigidBody2D>(entity).isStatic;
}

void Collider2DSystem::handleSolidEnter(Scene &scene, Entity entity, Entity otherEntity)
{
    // against a static body, resolveSolid only removes the velocity going into it
    if (!isDynamic(scene, entity) || !isDynamic(scene, otherEntity))
        return;
    Velocity &velocity = scene.getComponent<Velocity>(entity);
    Velocity &otherVelocity = scene.getComponent<Velocity>(otherEntity);
    const float mass = scene.getComponent<RigidBody2D>(entity).mass;
    const float otherMass = scene.getComponent<RigidBody2D>(otherEntity).mass;
    const float totalMass = mass + otherMass;
    // momentums added together, shared by both bodies
    const Velocity result{(mass * velocity.dx + otherMass * otherVelocity.dx) / totalMass,
                          (mass * velocity.dy + otherMass * otherVelocity.dy) / totalMass};
    velocity = result;
    otherVelocity = result;
}

void Collider2DSystem::resolveSolid(Scene &scene, Entity entityA, Transform &positionA,
                                    Entity entityB, Transform &positionB, const Contact &contact)
{
    const bool movesA = isDynamic(scene, entityA);
    const bool movesB = isDynamic(scene, entityB);
    // inverse masses: a body that cannot move counts as infinitely heavy
    const float inverseA = movesA ? 1.0f / scene.getComponent<RigidBody2D>(entityA).mass : 0.0f;
    const float inverseB = movesB ? 1.0f / scene.getComponent<RigidBody2D>(entityB).mass : 0.0f;
    const float inverseTotal = inverseA + inverseB;
    if (inverseTotal == 0.0f)
        return;

    // push apart, the lighter body moving the most
    const float pushA = contact.depth * inverseA / inverseTotal;
    const float pushB = contact.depth * inverseB / inverseTotal;
    positionA.x -= contact.normalX * pushA;
    positionA.y -= contact.normalY * pushA;
    positionB.x += contact.normalX * pushB;
    positionB.y += contact.normalY * pushB;

    // remove the relative velocity along the normal when the bodies are getting closer
    const Velocity zero{0.0f, 0.0f};
    const Velocity velocityA =
        scene.hasComponent<Velocity>(entityA) ? scene.getComponent<Velocity>(entityA) : zero;
    const Velocity velocityB =
        scene.hasComponent<Velocity>(entityB) ? scene.getComponent<Velocity>(entityB) : zero;
    const float approach = (velocityB.dx - velocityA.dx) * contact.normalX +
                           (velocityB.dy - velocityA.dy) * contact.normalY;
    if (approach >= 0.0f)
        return;
    const float impulse = -approach / inverseTotal;
    if (movesA) {
        Velocity &velocity = scene.getComponent<Velocity>(entityA);
        velocity.dx -= contact.normalX * impulse * inverseA;
        velocity.dy -= contact.normalY * impulse * inverseA;
    }
    if (movesB) {
        Velocity &velocity = scene.getComponent<Velocity>(entityB);
        velocity.dx += contact.normalX * impulse * inverseB;
        velocity.dy += contact.normalY * impulse * inverseB;
    }
}

std::optional<Contact> Collider2DSystem::checkCollisions(const Transform &positionA,
                                                         const Collider2D &colliderA,
                                                         const Transform &positionB,
                                                         const Collider2D &colliderB)
{
    return std::visit(
        [&](const auto &shapeA, const auto &shapeB) {
            return checkOverlaps(positionA, shapeA, positionB, shapeB);
        },
        colliderA.shape, colliderB.shape);
}

// Box vs Box: separated along the axis where they overlap the least
std::optional<Contact> Collider2DSystem::checkOverlaps(const Transform &positionA,
                                                       const BoxShape &colliderA,
                                                       const Transform &positionB,
                                                       const BoxShape &colliderB)
{
    const float dx = positionB.x - positionA.x;
    const float dy = positionB.y - positionA.y;
    const float overlapX = (colliderA.width + colliderB.width) / 2.0f - std::abs(dx);
    const float overlapY = (colliderA.height + colliderB.height) / 2.0f - std::abs(dy);

    if (overlapX <= 0.0f || overlapY <= 0.0f)
        return std::nullopt;
    if (overlapX < overlapY)
        return Contact{dx < 0.0f ? -1.0f : 1.0f, 0.0f, overlapX};
    return Contact{0.0f, dy < 0.0f ? -1.0f : 1.0f, overlapY};
}

// Circle vs Circle: separated along the line between the centers
std::optional<Contact> Collider2DSystem::checkOverlaps(const Transform &positionA,
                                                       const CircleShape &colliderA,
                                                       const Transform &positionB,
                                                       const CircleShape &colliderB)
{
    const float dx = positionB.x - positionA.x;
    const float dy = positionB.y - positionA.y;
    const float radiusSum = colliderA.radius + colliderB.radius;
    const float squaredDistance = dx * dx + dy * dy;

    // Compare squared distances to avoid a sqrt when they do not touch
    if (squaredDistance >= radiusSum * radiusSum)
        return std::nullopt;
    const float distance = std::sqrt(squaredDistance);
    if (distance == 0.0f)
        return Contact{0.0f, 1.0f, radiusSum};
    return Contact{dx / distance, dy / distance, radiusSum - distance};
}

// Box vs Circle: separated from the closest point of the box to the circle's center
std::optional<Contact> Collider2DSystem::checkOverlaps(const Transform &positionA,
                                                       const BoxShape &colliderA,
                                                       const Transform &positionB,
                                                       const CircleShape &colliderB)
{
    const float halfWidth = colliderA.width / 2.0f;
    const float halfHeight = colliderA.height / 2.0f;
    const float closestX =
        std::clamp(positionB.x, positionA.x - halfWidth, positionA.x + halfWidth);
    const float closestY =
        std::clamp(positionB.y, positionA.y - halfHeight, positionA.y + halfHeight);
    const float dx = positionB.x - closestX;
    const float dy = positionB.y - closestY;
    const float squaredDistance = dx * dx + dy * dy;

    if (squaredDistance >= colliderB.radius * colliderB.radius)
        return std::nullopt;
    if (squaredDistance > 0.0f) {
        const float distance = std::sqrt(squaredDistance);
        return Contact{dx / distance, dy / distance, colliderB.radius - distance};
    }
    // center inside the box: pushed out through the nearest side
    const float offsetX = positionB.x - positionA.x;
    const float offsetY = positionB.y - positionA.y;
    const float toSideX = halfWidth - std::abs(offsetX);
    const float toSideY = halfHeight - std::abs(offsetY);
    if (toSideX < toSideY)
        return Contact{offsetX < 0.0f ? -1.0f : 1.0f, 0.0f, toSideX + colliderB.radius};
    return Contact{0.0f, offsetY < 0.0f ? -1.0f : 1.0f, toSideY + colliderB.radius};
}

// Circle vs Box: the Box vs Circle contact, with the normal reversed
std::optional<Contact> Collider2DSystem::checkOverlaps(const Transform &positionA,
                                                       const CircleShape &colliderA,
                                                       const Transform &positionB,
                                                       const BoxShape &colliderB)
{
    std::optional<Contact> contact = checkOverlaps(positionB, colliderB, positionA, colliderA);
    if (contact) {
        contact->normalX = -contact->normalX;
        contact->normalY = -contact->normalY;
    }
    return contact;
}
