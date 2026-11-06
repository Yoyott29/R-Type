/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Collider2DSystem
*/

#pragma once
#include "Collider2D.hpp"
#include "Transform.hpp"
#include "RigidBody2D.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Velocity.hpp"
#include <algorithm>
#include <optional>
#include <set>
#include <utility>

// How two colliders overlap: normal (unit vector going from A to B) and depth (how far they are
// inside each other, in pixels). Moving B by normal * depth separates them
struct Contact {
    float normalX;
    float normalY;
    float depth;
};

class Collider2DSystem : public System
{
  public:
    void update(Scene &scene);
    std::optional<Contact> checkCollisions(const Transform &positionA, const Collider2D &colliderA,
                                           const Transform &positionB, const Collider2D &colliderB);

    // Box vs Box
    std::optional<Contact> checkOverlaps(const Transform &positionA, const BoxShape &colliderA,
                                         const Transform &positionB, const BoxShape &colliderB);
    // Circle vs Circle
    std::optional<Contact> checkOverlaps(const Transform &positionA, const CircleShape &colliderA,
                                         const Transform &positionB, const CircleShape &colliderB);
    // Box vs Circle
    std::optional<Contact> checkOverlaps(const Transform &positionA, const BoxShape &colliderA,
                                         const Transform &positionB, const CircleShape &colliderB);
    // Circle vs Box
    std::optional<Contact> checkOverlaps(const Transform &positionA, const CircleShape &colliderA,
                                         const Transform &positionB, const BoxShape &colliderB);

  private:
    // Trigger pair: calls the onCollision of each side whose mask accepts the other
    void handleTrigger(Entity entity, const Collider2D &collider, bool entityWants,
                       Entity otherEntity, const Collider2D &otherCollider, bool otherWants);
    // Solid pair that just started touching: the momentums of two moving bodies are added together
    void handleSolidEnter(Scene &scene, Entity entity, Entity otherEntity);
    // Solid pair touching on this step: pushes the moving bodies out of each other and removes
    // the velocity going into the contact, so bodies rest on each other (e.g. on a floor)
    void resolveSolid(Scene &scene, Entity entityA, Transform &positionA, Entity entityB,
                      Transform &positionB, const Contact &contact);
    // A RigidBody2D that is not static and has a Velocity to change
    bool isDynamic(Scene &scene, Entity entity) const;

    // Solid pairs (smallest entity first) touching during the previous step, so the momentums
    // are only added once per contact and not on every step the colliders overlap
    std::set<std::pair<Entity, Entity>> _solidContacts;
};
