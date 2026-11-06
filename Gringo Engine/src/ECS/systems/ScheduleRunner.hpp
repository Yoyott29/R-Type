/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ScheduleRunner
*/

#pragma once
#include "ScheduleGraph.hpp"
#include "ThreadPool.hpp"

class Scene;

// Runs every system of the graph once, as soon as its predecessors are done. Exclusive systems run
// on the caller, the others on the pool (the caller helps). The first exception thrown by a system
// is rethrown on the caller, once no system is running any more
void runParallel(const ScheduleGraph &graph, Scene &scene, ThreadPool &pool);
