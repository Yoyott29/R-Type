/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ThreadPool
*/

#include "ThreadPool.hpp"
#include <utility>

std::size_t ThreadPool::defaultWorkerCount()
{
    const std::size_t hardware = std::thread::hardware_concurrency();
    return hardware > 1 ? hardware - 1 : 1;
}

ThreadPool::ThreadPool(std::size_t workerCount)
{
    _workers.reserve(workerCount);
    for (std::size_t i = 0; i < workerCount; ++i)
        _workers.emplace_back([this] { workerLoop(); });
}

ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _stopping = true;
    }
    _workAvailable.notify_all();
    for (std::thread &worker : _workers)
        worker.join();
}

void ThreadPool::submit(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _tasks.push_back(std::move(task));
    }
    _workAvailable.notify_one();
    _progress.notify_all();
}

bool ThreadPool::runPendingTask()
{
    std::function<void()> task;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_tasks.empty())
            return false;
        task = popTask();
    }
    runTask(task);
    return true;
}

void ThreadPool::waitUntil(const std::function<bool()> &done)
{
    while (!done()) {
        if (runPendingTask())
            continue;
        std::unique_lock<std::mutex> lock(_mutex);
        _progress.wait(lock, [&] { return !_tasks.empty() || done(); });
    }
}

void ThreadPool::notifyAll()
{
    {
        // Empty critical section: a waiter is either before its predicate check or already
        // asleep, so this notify cannot be lost
        std::lock_guard<std::mutex> lock(_mutex);
    }
    _progress.notify_all();
}

std::size_t ThreadPool::workerCount() const
{
    return _workers.size();
}

void ThreadPool::workerLoop()
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _workAvailable.wait(lock, [this] { return _stopping || !_tasks.empty(); });
            if (_tasks.empty())
                return;
            task = popTask();
        }
        runTask(task);
    }
}

std::function<void()> ThreadPool::popTask()
{
    std::function<void()> task = std::move(_tasks.front());
    _tasks.pop_front();
    return task;
}

void ThreadPool::runTask(const std::function<void()> &task)
{
    try {
        task();
    } catch (...) {
        // Tasks report their own errors; an escaping one must not kill the thread
    }
    notifyAll();
}
