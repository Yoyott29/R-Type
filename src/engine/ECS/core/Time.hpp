/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Time
*/

#pragma once
#include <chrono>
#include <cstdint>
#include <ratio>

// Duree d'un tick : exactement 1/60 s, a additionner a un std::chrono::time_point
constexpr std::chrono::duration<std::int64_t, std::ratio<1, 60>> FIXED_TICK_DURATION{1};

// La meme duree en secondes (float), pour les calculs de gameplay (position += vitesse * dt)
constexpr float FIXED_DELTA_TIME = std::chrono::duration<float>(FIXED_TICK_DURATION).count();
