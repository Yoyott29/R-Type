/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ThreadPool
*/

#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
  public:
    explicit ThreadPool(std::size_t num_threads = 4);
    ~ThreadPool();

    void addTask(std::function<void()> task);
    void wait();
    std::size_t size() const { return _workers.size(); }

  private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks;
    std::mutex _mutex;
    std::condition_variable _cv;
    bool _shutdown = false;
    int _tasksCount = 0;

    void _workerLoop();
};
