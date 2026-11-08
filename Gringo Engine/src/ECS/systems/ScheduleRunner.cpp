/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ScheduleRunner
*/

#include "ScheduleRunner.hpp"
#include <atomic>
#include <deque>
#include <exception>
#include <mutex>
#include <optional>
#include <utility>

namespace
{
// Lives on the stack of runParallel. Pool tasks hold a pointer to it, which is safe because
// runParallel only returns once inFlight is 0, and a task touches nothing of it after its own
// inFlight decrement
class ScheduleRun
{
  public:
    ScheduleRun(const ScheduleGraph &graph, Scene &scene, ThreadPool &pool)
        : _graph(graph), _scene(scene), _pool(pool), _remaining(graph.systems.size())
    {
        for (std::size_t node = 0; node < _remaining.size(); ++node)
            _remaining[node] = graph.predecessorCount[node];
    }

    void run()
    {
        const std::size_t count = _graph.systems.size();
        for (std::size_t node = 0; node < count; ++node)
            if (_graph.predecessorCount[node] == 0)
                makeReady(node);

        while (_finished != count && !(_failed && _inFlight == 0)) {
            // Once failed, ready exclusive systems are skipped, so they must not wake this loop
            _pool.waitUntil([this, count] {
                return _finished == count || (!_failed && hasExclusive()) ||
                       (_failed && _inFlight == 0);
            });
            if (_failed)
                continue;
            if (const std::optional<std::size_t> node = popExclusive())
                runUnit(*node, false);
        }
        // finished == count can be seen before the last task has left
        _pool.waitUntil([this] { return _inFlight == 0; });
        if (_failed)
            std::rethrow_exception(_error);
    }

  private:
    void makeReady(std::size_t node)
    {
        if (_graph.exclusive[node]) {
            {
                const std::lock_guard<std::mutex> lock(_exclusiveMutex);
                _exclusiveQueue.push_back(node);
            }
            _pool.notifyAll();
            return;
        }
        if (_failed)
            return;
        // Before submit: inFlight must not reach 0 while this unit is queued
        ++_inFlight;
        _pool.submit([this, node] { runUnit(node, true); });
    }

    void runUnit(std::size_t node, bool pooled)
    {
        // A unit queued before the failure is skipped, not started
        if (!_failed) {
            try {
                const std::shared_ptr<System> &system = _graph.systems[node];
                system->runStart(_scene);
                system->update(_scene);
            } catch (...) {
                recordFailure(std::current_exception());
            }
        }
        complete(node, pooled);
    }

    void complete(std::size_t node, bool pooled)
    {
        for (const std::size_t successor : _graph.successors[node])
            if (--_remaining[successor] == 0)
                makeReady(successor);
        ++_finished;
        // Copied first: once inFlight is decremented, runParallel may return and destroy *this
        ThreadPool &pool = _pool;
        if (pooled)
            --_inFlight;
        pool.notifyAll();
    }

    void recordFailure(std::exception_ptr error)
    {
        const std::lock_guard<std::mutex> lock(_errorMutex);
        if (!_error)
            _error = std::move(error);
        _failed = true;
    }

    // Called under the pool mutex by waitUntil: locks only the queue mutex
    bool hasExclusive()
    {
        const std::lock_guard<std::mutex> lock(_exclusiveMutex);
        return !_exclusiveQueue.empty();
    }

    std::optional<std::size_t> popExclusive()
    {
        const std::lock_guard<std::mutex> lock(_exclusiveMutex);
        if (_exclusiveQueue.empty())
            return std::nullopt;
        const std::size_t node = _exclusiveQueue.front();
        _exclusiveQueue.pop_front();
        return node;
    }

    const ScheduleGraph &_graph;
    Scene &_scene;
    ThreadPool &_pool;
    std::vector<std::atomic<std::size_t>> _remaining;
    std::atomic<std::size_t> _finished = 0;
    std::atomic<std::size_t> _inFlight = 0; // pool units submitted and not finished yet
    std::deque<std::size_t> _exclusiveQueue;
    std::mutex _exclusiveMutex;
    std::atomic<bool> _failed = false;
    std::exception_ptr _error;
    std::mutex _errorMutex;
};
} // namespace

void runParallel(const ScheduleGraph &graph, Scene &scene, ThreadPool &pool)
{
    ScheduleRun run(graph, scene, pool);
    run.run();
}
