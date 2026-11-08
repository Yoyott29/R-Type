/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Collider2DTests
*/

#include "Check.hpp"
#include "Collider2D.hpp"
#include "Collider2DSystem.hpp"
#include "RigidBody2D.hpp"
#include "Scene.hpp"
#include "Transform.hpp"
#include "Velocity.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <set>
#include <utility>
#include <vector>

namespace
{
constexpr std::size_t kTriggerCount = 2000;
constexpr std::uint32_t kSeed = 42;
constexpr float kWorldSize = 2000.0f;
constexpr float kMinSize = 4.0f;
constexpr float kMaxSize = 60.0f;
constexpr std::uint32_t kLayerCount = 4;
constexpr std::size_t kFarEntityCount = 500;
constexpr float kFarOffset = 10000.0f;
constexpr float kFarSpacing = 100.0f;
constexpr float kBoxSize = 32.0f;
constexpr float kOverlap = 8.0f;
constexpr float kFloorWidth = 1000.0f;

using Calls = std::vector<std::pair<Entity, Entity>>;

std::shared_ptr<Collider2DSystem> makeScene(Scene &scene)
{
    scene.registerComponent<Transform>();
    scene.registerComponent<Collider2D>();
    scene.registerComponent<RigidBody2D>();
    scene.registerComponent<Velocity>();
    auto system = scene.registerSystem<Collider2DSystem>(FixedUpdate);
    ComponentSet requirements;
    requirements.set(scene.getComponentType<Transform>());
    requirements.set(scene.getComponentType<Collider2D>());
    scene.setSystemComponentSet<Collider2DSystem>(requirements);
    return system;
}

Entity addBox(Scene &scene, float x, float y, float size)
{
    const Entity entity = scene.createEntity();
    scene.addComponent(entity, Transform{x, y});
    Collider2D collider;
    collider.shape = BoxShape{size, size};
    scene.addComponent(entity, collider);
    return entity;
}

void addFarEntities(Scene &scene)
{
    for (std::size_t i = 0; i < kFarEntityCount; ++i)
        addBox(scene, kFarOffset + static_cast<float>(i) * kFarSpacing, kFarOffset, kBoxSize);
}

void addBody(Scene &scene, Entity entity, bool isStatic, Velocity velocity)
{
    scene.addComponent(entity, RigidBody2D{1.0f, isStatic, 0.0f});
    scene.addComponent(entity, velocity);
}

// The every-pair loop the broad phase replaced, for triggers only (they move nothing)
Calls bruteForceTriggers(Scene &scene, Collider2DSystem &system, const std::set<Entity> &entities)
{
    Calls calls;
    for (Entity entity : entities) {
        const Collider2D &collider = scene.getComponent<Collider2D>(entity);
        for (Entity other : entities) {
            if (other >= entity)
                break;
            const Collider2D &otherCollider = scene.getComponent<Collider2D>(other);
            const bool entityWants = (collider.mask & otherCollider.layer) != 0;
            const bool otherWants = (otherCollider.mask & collider.layer) != 0;
            if (!entityWants && !otherWants)
                continue;
            if (!system.checkCollisions(scene.getComponent<Transform>(entity), collider,
                                        scene.getComponent<Transform>(other), otherCollider))
                continue;
            if (entityWants)
                calls.emplace_back(entity, other);
            if (otherWants)
                calls.emplace_back(other, entity);
        }
    }
    return calls;
}
} // namespace

TEST_CASE(colliderTriggersMatchBruteForceInOrder)
{
    Scene scene;
    auto system = makeScene(scene);
    std::mt19937 random(kSeed);
    std::uniform_real_distribution<float> place(0.0f, kWorldSize);
    std::uniform_real_distribution<float> size(kMinSize, kMaxSize);
    std::uniform_int_distribution<std::uint32_t> layer(0, kLayerCount - 1);
    std::bernoulli_distribution coin;

    Calls calls;
    std::set<Entity> entities;
    for (std::size_t i = 0; i < kTriggerCount; ++i) {
        const Entity entity = scene.createEntity();
        scene.addComponent(entity, Transform{place(random), place(random)});
        Collider2D collider;
        if (coin(random))
            collider.shape = BoxShape{size(random), size(random)};
        else
            collider.shape = CircleShape{size(random) / 2.0f};
        collider.layer = 1u << layer(random);
        collider.mask = (1u << layer(random)) | (1u << layer(random));
        collider.isTrigger = true;
        collider.onCollision = [&calls](Entity self, Entity other) {
            calls.emplace_back(self, other);
        };
        scene.addComponent(entity, collider);
        entities.insert(entity);
    }

    const Calls expected = bruteForceTriggers(scene, *system, entities);
    system->update(scene);
    CHECK(!expected.empty());
    CHECK(calls == expected);
}

TEST_CASE(colliderSeparatesOverlappingBodiesAmongFarOnes)
{
    Scene scene;
    auto system = makeScene(scene);
    addFarEntities(scene);
    const Entity left = addBox(scene, 0.0f, 0.0f, kBoxSize);
    const Entity right = addBox(scene, kBoxSize - kOverlap, 0.0f, kBoxSize);
    addBody(scene, left, false, Velocity{1.0f, 0.0f});
    addBody(scene, right, false, Velocity{-1.0f, 0.0f});

    system->update(scene);
    const Transform &leftPosition = scene.getComponent<Transform>(left);
    const Transform &rightPosition = scene.getComponent<Transform>(right);
    CHECK(rightPosition.x - leftPosition.x >= kBoxSize);
    // equal masses, opposite velocities: the shared momentum is zero
    CHECK(scene.getComponent<Velocity>(left).dx == 0.0f);
    CHECK(scene.getComponent<Velocity>(right).dx == 0.0f);
}

TEST_CASE(colliderRestsBodyOnStaticFloor)
{
    Scene scene;
    auto system = makeScene(scene);
    addFarEntities(scene);
    const Entity floor = scene.createEntity();
    scene.addComponent(floor, Transform{0.0f, kBoxSize});
    Collider2D floorCollider;
    floorCollider.shape = BoxShape{kFloorWidth, kBoxSize};
    scene.addComponent(floor, floorCollider);
    addBody(scene, floor, true, Velocity{});
    const Entity body = addBox(scene, 0.0f, kOverlap, kBoxSize);
    const float fallSpeed = 5.0f;
    addBody(scene, body, false, Velocity{0.0f, fallSpeed});

    system->update(scene);
    CHECK(scene.getComponent<Transform>(floor).y == kBoxSize);
    CHECK(scene.getComponent<Transform>(body).y <= 0.0f);
    CHECK(scene.getComponent<Velocity>(body).dy <= 0.0f);
}
