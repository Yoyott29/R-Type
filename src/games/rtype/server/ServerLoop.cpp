/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ServerLoop
*/

#include "ServerLoop.hpp"
#include "AppAlive.hpp"
#include "Time.hpp"
#include <atomic>
#include <csignal>
#include <thread>

namespace
{
std::atomic<bool> g_stopRequested{false};

void onStopSignal(int) { g_stopRequested.store(true); }
} // namespace

void installStopSignals()
{
    std::signal(SIGINT, onStopSignal);
    std::signal(SIGTERM, onStopSignal);
}

void StopOnSignalSystem::update(Scene &scene)
{
    if (g_stopRequested.load())
        scene.getResource<AppAlive>().alive = false;
}

void FrameLimiterSystem::Start(Scene &) { _next = NetClock::now(); }

void FrameLimiterSystem::update(Scene &)
{
    _next += std::chrono::duration_cast<NetClock::duration>(FIXED_TICK_DURATION);
    auto now = NetClock::now();
    if (_next < now)
        _next = now;
    std::this_thread::sleep_until(_next);
}
