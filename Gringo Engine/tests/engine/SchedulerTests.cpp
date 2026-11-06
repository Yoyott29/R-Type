/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SchedulerTests
*/

#include "Check.hpp"
#include "Scene.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <latch>
#include <memory>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#ifdef __linux__
#include <filesystem>
#include <iterator>
#endif

namespace
{
struct Position {
};
struct Speed {
};

enum Slot : std::size_t { First, Second, Third, Fourth, SlotCount };

constexpr auto kOverlapTimeout = std::chrono::seconds(2);
constexpr auto kSlowSystemDelay = std::chrono::milliseconds(50);
constexpr std::ptrdiff_t kOverlapThreads = 2;
constexpr std::size_t kUndeclaredCount = 3;
constexpr std::size_t kChainLength = 8;
constexpr std::size_t kPhasedCount = 2;
constexpr int kAlone = 1;
#ifdef __linux__
constexpr std::size_t kMainThreadOnly = 1;
#endif

// Arrives at the latch, then spins until every other thread arrived. False on timeout, which is
// what a sequential scheduler gives instead of a deadlock
bool arriveAndWait(std::latch &latch)
{
    latch.count_down();
    const auto deadline = std::chrono::steady_clock::now() + kOverlapTimeout;
    while (!latch.try_wait()) {
        if (std::chrono::steady_clock::now() >= deadline)
            return false;
        std::this_thread::yield();
    }
    return true;
}

std::vector<std::size_t> sequence(std::size_t count)
{
    std::vector<std::size_t> result(count);
    std::iota(result.begin(), result.end(), 0);
    return result;
}

struct NoopSystem : System {
    void update(Scene &) override {}
};

// Every system records (index, thread) in run order
struct OrderLog {
    static inline std::mutex mutex;
    static inline std::vector<std::size_t> indices;
    static inline std::vector<std::thread::id> threads;

    static void reset()
    {
        indices.clear();
        threads.clear();
    }
};

template <std::size_t Index> struct RecordingSystem : System {
    void update(Scene &) override
    {
        const std::lock_guard<std::mutex> lock(OrderLog::mutex);
        OrderLog::indices.push_back(Index);
        OrderLog::threads.push_back(std::this_thread::get_id());
    }
};

template <std::size_t... Indices>
void registerChain(Scene &scene, const SystemAccess &access, std::index_sequence<Indices...>)
{
    (scene.registerSystem<RecordingSystem<Indices>>(), ...);
    (scene.setSystemAccess<RecordingSystem<Indices>>(access), ...);
}

struct OverlapState {
    static inline std::unique_ptr<std::latch> latch;
    static inline std::array<std::atomic<bool>, SlotCount> overlapped = {};
    static inline std::atomic<bool> overlapPhase = false;

    static void reset()
    {
        latch = std::make_unique<std::latch>(kOverlapThreads);
        for (std::atomic<bool> &flag : overlapped)
            flag = false;
    }
};

template <std::size_t Index> struct LatchSystem : System {
    void update(Scene &) override { OverlapState::overlapped[Index] = arriveAndWait(*OverlapState::latch); }
};

// Records its index in the first phase, waits on the latch in the overlap phase
template <std::size_t Index> struct PhasedSystem : System {
    void update(Scene &) override
    {
        if (OverlapState::overlapPhase) {
            OverlapState::overlapped[Index] = arriveAndWait(*OverlapState::latch);
            return;
        }
        const std::lock_guard<std::mutex> lock(OrderLog::mutex);
        OrderLog::indices.push_back(Index);
    }
};

// Start and end ticks of every system, from one shared clock
struct Timeline {
    static inline std::atomic<int> running = 0;
    static inline std::atomic<std::size_t> clock = 0;
    static inline std::array<std::atomic<std::size_t>, SlotCount> starts = {};
    static inline std::array<std::atomic<std::size_t>, SlotCount> ends = {};
    static inline std::thread::id exclusiveThread;
    static inline std::atomic<bool> exclusiveAlone = false;
};

template <std::size_t Index, bool Exclusive> struct TimedSystem : System {
    void update(Scene &) override
    {
        const int runningNow = ++Timeline::running;
        Timeline::starts[Index] = Timeline::clock++;
        if constexpr (Exclusive) {
            Timeline::exclusiveThread = std::this_thread::get_id();
            Timeline::exclusiveAlone = runningNow == kAlone;
        }
        Timeline::ends[Index] = Timeline::clock++;
        --Timeline::running;
    }
};

struct StartLog {
    static inline std::mutex mutex;
    static inline std::vector<std::string> entries;
};

struct StartingSystem : System {
    void Start(Scene &) override
    {
        const std::lock_guard<std::mutex> lock(StartLog::mutex);
        StartLog::entries.emplace_back("start");
    }
    void update(Scene &) override
    {
        const std::lock_guard<std::mutex> lock(StartLog::mutex);
        StartLog::entries.emplace_back("update");
    }
};

struct ExceptionState {
    static inline std::atomic<bool> dependentRan = false;
    static inline std::atomic<bool> slowStarted = false;
    static inline std::atomic<bool> slowFinished = false;
};

struct ThrowingSystem : System {
    void update(Scene &) override { throw std::runtime_error("system failed"); }
};

struct DependentSystem : System {
    void update(Scene &) override { ExceptionState::dependentRan = true; }
};

// Throws once the slow system is running, so the slow one is in flight when the error happens
struct ThrowWhileSlowRunsSystem : System {
    void update(Scene &) override
    {
        const auto deadline = std::chrono::steady_clock::now() + kOverlapTimeout;
        while (!ExceptionState::slowStarted && std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        throw std::runtime_error("system failed");
    }
};

struct SlowSystem : System {
    void update(Scene &) override
    {
        ExceptionState::slowStarted = true;
        std::this_thread::sleep_for(kSlowSystemDelay);
        ExceptionState::slowFinished = true;
    }
};

struct RemovalState {
    static inline bool removed = false;
    static inline std::atomic<bool> removedRan = false;
};

struct RemovedSystem : System {
    void update(Scene &) override { RemovalState::removedRan = true; }
};

struct RemoverSystem : System {
    void update(Scene &scene) override
    {
        if (RemovalState::removed)
            return;
        scene.removeSystem<RemovedSystem>();
        RemovalState::removed = true;
    }
};

#ifdef __linux__
std::size_t threadCount()
{
    const std::filesystem::directory_iterator tasks("/proc/self/task");
    return static_cast<std::size_t>(std::distance(begin(tasks), end(tasks)));
}

// The threads of earlier tests are joined, but can stay listed for a moment after the join
bool settleToMainThread()
{
    const auto deadline = std::chrono::steady_clock::now() + kOverlapTimeout;
    while (threadCount() != kMainThreadOnly) {
        if (std::chrono::steady_clock::now() >= deadline)
            return false;
        std::this_thread::yield();
    }
    return true;
}
#endif
} // namespace

TEST_CASE(undeclaredSystemsRunInOrderOnMainThread)
{
    Scene scene;
    OrderLog::reset();
    scene.registerSystem<RecordingSystem<First>>();
    scene.registerSystem<RecordingSystem<Second>>();
    scene.registerSystem<RecordingSystem<Third>>();
    scene.runSchedule(Update);
    CHECK(OrderLog::indices == sequence(kUndeclaredCount));
    for (const std::thread::id id : OrderLog::threads)
        CHECK(id == std::this_thread::get_id());
}

TEST_CASE(independentSystemsOverlap)
{
    Scene scene;
    OverlapState::reset();
    scene.registerSystem<LatchSystem<First>>();
    scene.registerSystem<LatchSystem<Second>>();
    scene.setSystemAccess<LatchSystem<First>>(scene.access());
    scene.setSystemAccess<LatchSystem<Second>>(scene.access());
    scene.runSchedule(Update);
    CHECK(OverlapState::overlapped[First]);
    CHECK(OverlapState::overlapped[Second]);
}

TEST_CASE(conflictingSystemsKeepOrder)
{
    Scene scene;
    OrderLog::reset();
    scene.registerComponent<Position>();
    registerChain(scene, scene.access().write<Position>(), std::make_index_sequence<kChainLength>{});
    scene.registerSystem<NoopSystem>();
    scene.setSystemAccess<NoopSystem>(scene.access());
    scene.runSchedule(Update);
    CHECK(OrderLog::indices == sequence(kChainLength));
}

TEST_CASE(exclusiveRunsAloneOnMainThread)
{
    Scene scene;
    Timeline::running = 0;
    Timeline::clock = 0;
    Timeline::exclusiveAlone = false;
    scene.registerSystem<TimedSystem<First, false>>();
    scene.registerSystem<TimedSystem<Second, false>>();
    scene.registerSystem<TimedSystem<Third, true>>();
    scene.registerSystem<TimedSystem<Fourth, false>>();
    scene.setSystemAccess<TimedSystem<First, false>>(scene.access());
    scene.setSystemAccess<TimedSystem<Second, false>>(scene.access());
    scene.setSystemAccess<TimedSystem<Fourth, false>>(scene.access());
    scene.runSchedule(Update);
    CHECK(Timeline::exclusiveThread == std::this_thread::get_id());
    CHECK(Timeline::exclusiveAlone);
    CHECK(Timeline::ends[First] < Timeline::starts[Third]);
    CHECK(Timeline::ends[Second] < Timeline::starts[Third]);
    CHECK(Timeline::ends[Third] < Timeline::starts[Fourth]);
}

TEST_CASE(startRunsOnceBeforeUpdate)
{
    Scene scene;
    StartLog::entries.clear();
    scene.registerSystem<StartingSystem>();
    scene.registerSystem<NoopSystem>();
    scene.setSystemAccess<StartingSystem>(scene.access());
    scene.setSystemAccess<NoopSystem>(scene.access());
    scene.runSchedule(Update);
    scene.runSchedule(Update);
    CHECK(StartLog::entries == std::vector<std::string>({"start", "update", "update"}));
}

TEST_CASE(parallelExceptionIsRethrownOnMainThread)
{
    Scene scene;
    ExceptionState::dependentRan = false;
    scene.registerComponent<Position>();
    scene.registerComponent<Speed>();
    scene.registerSystem<ThrowingSystem>();
    scene.registerSystem<DependentSystem>();
    scene.registerSystem<NoopSystem>();
    scene.setSystemAccess<ThrowingSystem>(scene.access().write<Position>());
    scene.setSystemAccess<DependentSystem>(scene.access().read<Position>());
    scene.setSystemAccess<NoopSystem>(scene.access().write<Speed>());
    CHECK_THROWS(scene.runSchedule(Update), std::runtime_error);
    CHECK(!ExceptionState::dependentRan);
}

TEST_CASE(exceptionWaitsForInFlight)
{
    Scene scene;
    ExceptionState::slowStarted = false;
    ExceptionState::slowFinished = false;
    scene.registerSystem<ThrowWhileSlowRunsSystem>();
    scene.registerSystem<SlowSystem>();
    scene.setSystemAccess<ThrowWhileSlowRunsSystem>(scene.access());
    scene.setSystemAccess<SlowSystem>(scene.access());
    CHECK_THROWS(scene.runSchedule(Update), std::runtime_error);
    CHECK(ExceptionState::slowFinished);
}

TEST_CASE(removedSystemStillRunsThisFrame)
{
    Scene scene;
    RemovalState::removed = false;
    RemovalState::removedRan = false;
    scene.registerSystem<RemoverSystem>();
    scene.registerSystem<RemovedSystem>();
    scene.registerSystem<NoopSystem>();
    scene.setSystemAccess<RemovedSystem>(scene.access());
    scene.setSystemAccess<NoopSystem>(scene.access());
    scene.runSchedule(Update);
    CHECK(RemovalState::removedRan);
    RemovalState::removedRan = false;
    scene.runSchedule(Update);
    CHECK(!RemovalState::removedRan);
}

TEST_CASE(emptyScheduleIsNoOp)
{
    Scene scene;
    scene.runSchedule(Render);
}

TEST_CASE(accessChangeRebuildsGraph)
{
    Scene scene;
    OrderLog::reset();
    OverlapState::reset();
    OverlapState::overlapPhase = false;
    scene.registerComponent<Position>();
    scene.registerSystem<PhasedSystem<First>>();
    scene.registerSystem<PhasedSystem<Second>>();
    scene.setSystemAccess<PhasedSystem<First>>(scene.access().write<Position>());
    scene.setSystemAccess<PhasedSystem<Second>>(scene.access().write<Position>());
    scene.runSchedule(Update);
    CHECK(OrderLog::indices == sequence(kPhasedCount));

    scene.setSystemAccess<PhasedSystem<First>>(scene.access());
    scene.setSystemAccess<PhasedSystem<Second>>(scene.access());
    OverlapState::overlapPhase = true;
    scene.runSchedule(Update);
    CHECK(OverlapState::overlapped[First]);
    CHECK(OverlapState::overlapped[Second]);
}

#ifdef __linux__
// Each thread of the process is one entry of /proc/self/task
TEST_CASE(sequentialScheduleCreatesNoThread)
{
    OrderLog::reset();
    CHECK(settleToMainThread());
    Scene sequential;
    sequential.registerSystem<RecordingSystem<First>>();
    sequential.registerSystem<RecordingSystem<Second>>();
    sequential.runSchedule(Update);
    CHECK(threadCount() == kMainThreadOnly);

    // The probe does see a pool once a schedule has parallelism
    Scene parallel;
    parallel.registerSystem<RecordingSystem<First>>();
    parallel.registerSystem<RecordingSystem<Second>>();
    parallel.setSystemAccess<RecordingSystem<First>>(parallel.access());
    parallel.setSystemAccess<RecordingSystem<Second>>(parallel.access());
    parallel.runSchedule(Update);
    CHECK(threadCount() > kMainThreadOnly);
}

TEST_CASE(unregisteredScheduleCreatesNoThread)
{
    CHECK(settleToMainThread());
    Scene scene;
    scene.runSchedule(Render);
    CHECK(threadCount() == kMainThreadOnly);
}
#endif
