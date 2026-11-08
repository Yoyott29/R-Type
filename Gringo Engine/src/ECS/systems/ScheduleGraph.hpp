/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ScheduleGraph
*/

#pragma once
#include "System.hpp"
#include "SystemAccess.hpp"
#include <cstddef>
#include <memory>
#include <vector>

// Dependency graph of a schedule. Edge i -> j means j must wait for i (i is registered before j)
struct ScheduleGraph {
    std::vector<std::shared_ptr<System>> systems; // registration order
    std::vector<bool> exclusive;
    std::vector<std::vector<std::size_t>> successors;
    std::vector<std::size_t> predecessorCount;
    bool hasParallelism = false; // true if some system can run beside its neighbour
};

// accesses[i] == nullptr means systems[i] is exclusive
ScheduleGraph buildScheduleGraph(std::vector<std::shared_ptr<System>> systems,
                                 const std::vector<const SystemAccess *> &accesses);
