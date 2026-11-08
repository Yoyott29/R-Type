/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ParallelFor
*/

#pragma once
#include "ThreadPool.hpp"
#include <cstddef>
#include <functional>

// Below this many items per chunk, splitting costs more than it saves
constexpr std::size_t PARALLEL_FOR_MIN_CHUNK = 64;

using ChunkBody = std::function<void(std::size_t begin, std::size_t end)>;

// Splits [0, count) into at most workerCount() + 1 contiguous chunks of at
// least minChunk items (fewer when count is small), runs chunk 0 on the calling
// thread and the others on the pool, and returns once all are done. The calling
// thread runs queued tasks while it waits, so it is safe from inside a pool
// task. The first exception thrown by a chunk is rethrown after every chunk
// ended; chunks that had not started by then are skipped. body must be safe to
// run concurrently on disjoint ranges
void parallelFor(ThreadPool &pool, std::size_t count, std::size_t minChunk, const ChunkBody &body);
