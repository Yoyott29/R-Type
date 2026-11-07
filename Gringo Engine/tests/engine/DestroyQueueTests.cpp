/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** DestroyQueueTests
*/

#include "Check.hpp"
#include "Scene.hpp"
#include <cstddef>
#include <thread>
#include <vector>

namespace
{
constexpr std::size_t kDestroyedCount = 1000;
constexpr std::size_t kDestroyThreads = 4;
} // namespace

TEST_CASE(concurrentQueueDestroyLosesNothing)
{
    Scene scene;
    std::vector<Entity> entities;
    for (std::size_t i = 0; i < kDestroyedCount; ++i)
        entities.push_back(scene.createEntity());

    const std::size_t share = kDestroyedCount / kDestroyThreads;
    {
        std::vector<std::jthread> threads;
        for (std::size_t t = 0; t < kDestroyThreads; ++t)
            threads.emplace_back([&scene, &entities, t, share] {
                for (std::size_t i = t * share; i < (t + 1) * share; ++i)
                    scene.queueDestroy(entities[i]);
            });
    }

    scene.flushDestroyQueue();
    for (Entity entity : entities)
        CHECK(!scene.isEntityAlive(entity));
}

TEST_CASE(queueDestroyTwiceIsHarmless)
{
    Scene scene;
    const Entity entity = scene.createEntity();
    scene.queueDestroy(entity);
    scene.queueDestroy(entity);
    scene.flushDestroyQueue();
    CHECK(!scene.isEntityAlive(entity));
}
