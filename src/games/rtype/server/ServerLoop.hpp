/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ServerLoop
*/

#pragma once
#include "NetworkConfig.hpp"
#include "Scene.hpp"
#include "System.hpp"

// SIGINT/SIGTERM set a flag that StopOnSignalSystem turns into AppAlive.alive = false
void installStopSignals();

class StopOnSignalSystem : public System
{
  public:
    void update(Scene &scene) override;
};

// Application::run never sleeps: keeps the headless server loop at 60 Hz
class FrameLimiterSystem : public System
{
  public:
    void Start(Scene &) override;
    void update(Scene &scene) override;

    NetClock::time_point _next;
};
