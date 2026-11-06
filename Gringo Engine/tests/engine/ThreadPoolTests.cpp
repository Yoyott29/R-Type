/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ThreadPoolTests
*/

#include "Check.hpp"
#include "ThreadPool.hpp"
#include <atomic>
#include <cstddef>
#include <latch>
#include <stdexcept>
#include <thread>

namespace
{
constexpr std::size_t kTaskCount = 64;
constexpr std::size_t kSingleWorker = 1;
constexpr std::ptrdiff_t kLatchCount = 1;
} // namespace

TEST_CASE(defaultWorkerCountIsAtLeastOne)
{
    CHECK(ThreadPool::defaultWorkerCount() >= 1);
}

TEST_CASE(submittedTasksAllRunOnWorkers)
{
    ThreadPool pool;
    std::atomic<std::size_t> counter{0};

    for (std::size_t i = 0; i < kTaskCount; ++i)
        pool.submit([&] { ++counter; });
    pool.waitUntil([&] { return counter.load() == kTaskCount; });
    CHECK(counter.load() == kTaskCount);
}

TEST_CASE(runPendingTaskRunsOnCaller)
{
    ThreadPool pool(kSingleWorker);
    std::latch started(kLatchCount);
    std::latch release(kLatchCount);
    std::thread::id recorded;

    pool.submit([&] {
        started.count_down();
        release.wait();
    });
    started.wait();
    pool.submit([&] { recorded = std::this_thread::get_id(); });
    CHECK(pool.runPendingTask());
    CHECK(recorded == std::this_thread::get_id());
    release.count_down();
    CHECK(!pool.runPendingTask());
}

TEST_CASE(waitUntilHelpsWhenWorkersAreBusy)
{
    ThreadPool pool(kSingleWorker);
    std::latch started(kLatchCount);
    std::atomic<bool> flag{false};

    pool.submit([&] {
        started.count_down();
        while (!flag.load())
            std::this_thread::yield();
    });
    started.wait();
    pool.submit([&] { flag = true; });
    pool.waitUntil([&] { return flag.load(); });
    CHECK(flag.load());
}

TEST_CASE(throwingTaskDoesNotKillWorker)
{
    ThreadPool pool(kSingleWorker);
    std::atomic<bool> done{false};

    pool.submit([] { throw std::runtime_error("boom"); });
    pool.submit([&] { done = true; });
    pool.waitUntil([&] { return done.load(); });
    CHECK(done.load());
}

TEST_CASE(destructorRunsQueuedTasks)
{
    std::atomic<std::size_t> counter{0};
    {
        ThreadPool pool(kSingleWorker);
        for (std::size_t i = 0; i < kTaskCount; ++i)
            pool.submit([&] { ++counter; });
    }
    CHECK(counter.load() == kTaskCount);
}
