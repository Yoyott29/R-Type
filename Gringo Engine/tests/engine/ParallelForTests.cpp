/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ParallelForTests
*/

#include "Check.hpp"
#include "ParallelFor.hpp"
#include "Scene.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace
{
constexpr std::size_t kItemCount = 10000;
constexpr std::size_t kMinChunk = 16;
constexpr std::size_t kOverlapThreads = 2;
constexpr std::size_t kFailingChunk = 0;
constexpr auto kSlowChunkDelay = std::chrono::milliseconds(50);

enum Slot : std::size_t { First, Second };

struct HitCounter {
    static inline std::atomic<std::size_t> hits{0};
};

template <std::size_t Index> struct NestedSystem : System {
    void update(Scene &scene) override
    {
        scene.parallelFor(kItemCount, [](std::size_t begin, std::size_t end) {
            HitCounter::hits += end - begin;
        });
    }
};
} // namespace

TEST_CASE(coversEveryIndexOnce)
{
    ThreadPool pool;
    std::vector<std::atomic<int>> hits(kItemCount);
    parallelFor(pool, kItemCount, kMinChunk, [&](std::size_t begin, std::size_t end) {
        for (std::size_t i = begin; i < end; ++i)
            ++hits[i];
    });
    for (std::size_t i = 0; i < kItemCount; ++i)
        CHECK(hits[i] == 1);
}

TEST_CASE(chunksAreContiguousAndBounded)
{
    ThreadPool pool;
    std::mutex mutex;
    std::vector<std::pair<std::size_t, std::size_t>> chunks;
    parallelFor(pool, kItemCount, kMinChunk, [&](std::size_t begin, std::size_t end) {
        const std::lock_guard<std::mutex> lock(mutex);
        chunks.emplace_back(begin, end);
    });
    std::sort(chunks.begin(), chunks.end());
    CHECK(chunks.size() <= pool.workerCount() + 1);
    std::size_t expected = 0;
    for (const auto &[begin, end] : chunks) {
        CHECK(begin == expected);
        CHECK(end > begin);
        expected = end;
    }
    CHECK(expected == kItemCount);
}

TEST_CASE(smallCountRunsInlineWithoutPool)
{
    Scene scene;
    std::size_t calls = 0;
    std::size_t seenBegin = 1;
    std::size_t seenEnd = 0;
    std::thread::id seenThread;
    scene.parallelFor(PARALLEL_FOR_MIN_CHUNK, [&](std::size_t begin, std::size_t end) {
        ++calls;
        seenBegin = begin;
        seenEnd = end;
        seenThread = std::this_thread::get_id();
    });
    CHECK(calls == 1);
    CHECK(seenBegin == 0);
    CHECK(seenEnd == PARALLEL_FOR_MIN_CHUNK);
    CHECK(seenThread == std::this_thread::get_id());
    CHECK(!scene.hasThreadPool());
}

TEST_CASE(zeroCountCallsNothing)
{
    Scene scene;
    bool called = false;
    scene.parallelFor(0, [&](std::size_t, std::size_t) { called = true; });
    CHECK(!called);
    CHECK(!scene.hasThreadPool());
}

TEST_CASE(chunkExceptionIsRethrownOnCaller)
{
    ThreadPool pool;
    CHECK_THROWS(parallelFor(pool, kItemCount, kMinChunk,
                             [](std::size_t begin, std::size_t) {
                                 if (begin == kFailingChunk)
                                     throw std::runtime_error("chunk failed");
                             }),
                 std::runtime_error);
}

TEST_CASE(parallelForWaitsForChunksBeforeRethrow)
{
    ThreadPool pool;
    std::atomic<std::size_t> started{0};
    std::atomic<std::size_t> finished{0};
    bool caught = false;
    try {
        parallelFor(pool, kItemCount, kMinChunk, [&](std::size_t begin, std::size_t) {
            if (begin == kFailingChunk) {
                // Fail only once another chunk is running, so the wait has
                // something to wait for
                while (started == 0)
                    std::this_thread::yield();
                throw std::runtime_error("chunk failed");
            }
            ++started;
            std::this_thread::sleep_for(kSlowChunkDelay);
            ++finished;
        });
    } catch (const std::runtime_error &) {
        caught = true;
    }
    CHECK(caught);
    CHECK(started > 0);
    CHECK(finished == started);
}

TEST_CASE(nestedParallelForCompletes)
{
    Scene scene;
    HitCounter::hits = 0;
    scene.registerSystem<NestedSystem<First>>();
    scene.registerSystem<NestedSystem<Second>>();
    scene.setSystemAccess<NestedSystem<First>>(scene.access());
    scene.setSystemAccess<NestedSystem<Second>>(scene.access());
    scene.runSchedule(Update);
    CHECK(HitCounter::hits == kOverlapThreads * kItemCount);
}

TEST_CASE(vectorOverloadVisitsEachItem)
{
    Scene scene;
    std::vector<Entity> items(kItemCount);
    std::iota(items.begin(), items.end(), Entity{0});
    std::atomic<std::size_t> sum{0};
    scene.parallelFor(items, [&](const Entity &item) { sum += item; });
    const std::size_t expected = std::accumulate(items.begin(), items.end(), std::size_t{0});
    CHECK(sum == expected);
}
