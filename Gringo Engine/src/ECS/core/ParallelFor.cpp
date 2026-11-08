/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ParallelFor
*/

#include "ParallelFor.hpp"
#include <algorithm>
#include <atomic>
#include <exception>
#include <mutex>
#include <utility>

namespace
{
// Lives on the caller's stack: parallelFor does not return before every chunk
// is done
struct ForRun {
    ThreadPool &pool;
    const ChunkBody &body;
    std::size_t count;
    std::size_t chunkCount;

    ForRun(ThreadPool &pool, const ChunkBody &body, std::size_t count, std::size_t chunkCount)
        : pool(pool), body(body), count(count), chunkCount(chunkCount)
    {
    }

    std::atomic<std::size_t> done{0};
    std::atomic<bool> failed{false};
    std::mutex errorMutex;
    std::exception_ptr error;

    std::size_t chunkBegin(std::size_t chunk) const { return count * chunk / chunkCount; }

    void runChunk(std::size_t chunk)
    {
        if (!failed) {
            try {
                body(chunkBegin(chunk), chunkBegin(chunk + 1));
            } catch (...) {
                recordFailure(std::current_exception());
            }
        }
        complete();
    }

    void recordFailure(std::exception_ptr exception)
    {
        const std::lock_guard<std::mutex> lock(errorMutex);
        if (!error)
            error = std::move(exception);
        failed = true;
    }

    void complete()
    {
        // Copied first: once done is incremented, parallelFor may return and
        // destroy *this
        ThreadPool &poolRef = pool;
        ++done;
        poolRef.notifyAll();
    }
};
} // namespace

void parallelFor(ThreadPool &pool, std::size_t count, std::size_t minChunk, const ChunkBody &body)
{
    if (count == 0)
        return;
    const std::size_t chunkSize = std::max<std::size_t>(minChunk, 1);
    const std::size_t wanted = (count + chunkSize - 1) / chunkSize;
    const std::size_t chunkCount = std::min(wanted, pool.workerCount() + 1);
    ForRun run{pool, body, count, chunkCount};
    for (std::size_t chunk = 1; chunk < chunkCount; ++chunk)
        pool.submit([&run, chunk] { run.runChunk(chunk); });
    run.runChunk(0);
    pool.waitUntil([&run, chunkCount] { return run.done == chunkCount; });
    if (run.error)
        std::rethrow_exception(run.error);
}
