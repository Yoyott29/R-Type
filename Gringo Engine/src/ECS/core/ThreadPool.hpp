/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ThreadPool
*/

#pragma once
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

// Fixed set of worker threads fed by one shared queue. Tasks must not let exceptions escape: an
// escaping one is reported on stderr, then dropped. A thread waiting in waitUntil runs queued
// tasks instead of idling
class ThreadPool
{
  public:
    // hardware_concurrency() - 1, at least 1: the calling thread works too
    static std::size_t defaultWorkerCount();

    explicit ThreadPool(std::size_t workerCount = defaultWorkerCount());
    // Runs the tasks still queued, then joins the workers
    ~ThreadPool();
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    void submit(std::function<void()> task);
    // Runs one queued task on the calling thread. False if the queue was empty
    bool runPendingTask();
    // Returns once done() is true, running queued tasks meanwhile. done() is re-evaluated after
    // each task and after notifyAll(). done() is evaluated with the pool mutex held, so it must not
    // call into the pool
    void waitUntil(const std::function<bool()> &done);
    // To call after changing what a waitUntil predicate reads
    void notifyAll();
    std::size_t workerCount() const;

  private:
    void workerLoop();
    // _mutex held, queue not empty
    std::function<void()> popTask();
    void runTask(const std::function<void()> &task);

    std::deque<std::function<void()>> _tasks;
    std::mutex _mutex;
    std::condition_variable _workAvailable;
    std::condition_variable _progress;
    bool _stopping = false;
    std::vector<std::thread> _workers;
};
