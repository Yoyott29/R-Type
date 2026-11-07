/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ThreadPool
*/

#include "ThreadPool.hpp"
#include <exception>
#include <iostream>
#include <utility>

std::size_t ThreadPool::defaultWorkerCount()
{
    constexpr std::size_t CALLER_THREADS = 1;
    constexpr std::size_t MIN_WORKERS = 1;
    const std::size_t hardware = std::thread::hardware_concurrency();
    return hardware > CALLER_THREADS ? hardware - CALLER_THREADS : MIN_WORKERS;
}

ThreadPool::ThreadPool(std::size_t workerCount) : _workerCount(workerCount)
{
    _workers.reserve(workerCount);
    for (std::size_t i = 0; i < workerCount; ++i)
        _workers.emplace_back([this](std::stop_token token) { workerLoop(token); });
}

ThreadPool::~ThreadPool() = default;

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
    return _workerCount;
}

void ThreadPool::workerLoop(std::stop_token token)
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            // Returns false only once a stop is requested and the queue is drained
            if (!_workAvailable.wait(lock, token, [this] { return !_tasks.empty(); }))
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
    } catch (const std::exception &error) {
        std::cerr << "ThreadPool: task threw: " << error.what() << '\n';
    } catch (...) {
        std::cerr << "ThreadPool: task threw a non-std exception\n";
    }
    notifyAll();
}
