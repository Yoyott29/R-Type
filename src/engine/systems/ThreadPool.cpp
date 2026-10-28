/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ThreadPool
*/

#include "ThreadPool.hpp"

ThreadPool::ThreadPool(std::size_t num_threads)
{
    for (std::size_t i = 0; i < num_threads; ++i) {
        _workers.emplace_back([this] { _workerLoop(); });
    }
}

ThreadPool::~ThreadPool()
{
    std::unique_lock lock(_mutex);
    _shutdown = true;
    _cv.notify_all();
    lock.unlock();

    for (auto &worker : _workers) {
        worker.join();
    }
}

void ThreadPool::addTask(std::function<void()> task)
{
    std::unique_lock lock(_mutex);
    _tasks.push(std::move(task));
    _tasksCount++;
    _cv.notify_one();
}

void ThreadPool::wait()
{
    std::unique_lock lock(_mutex);
    _cv.wait(lock, [this] { return _tasksCount == 0; });
}

void ThreadPool::_workerLoop()
{
    while (true) {
        std::unique_lock lock(_mutex);
        _cv.wait(lock, [this] { return !_tasks.empty() || _shutdown; });

        if (_shutdown && _tasks.empty())
            break;

        if (!_tasks.empty()) {
            auto task = std::move(_tasks.front());
            _tasks.pop();
            lock.unlock();

            task();

            lock.lock();
            _tasksCount--;
            _cv.notify_all();
        }
    }
}
