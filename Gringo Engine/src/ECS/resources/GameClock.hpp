/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** GameClock
*/

#pragma once
#include <cstdint>

// Scene resource: the number of fixed ticks (FixedUpdate, 1/60 s each) run since
// Application::run started. Updated by the application after every tick
struct GameClock {
    std::uint64_t tick = 0;
};
