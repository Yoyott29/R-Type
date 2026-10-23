/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** TickTimeManager
*/

#pragma once
#include "Time.hpp"
#include <chrono>
#include <cstdint>

// Garde le rythme des ticks, partage par le client et le serveur. Ne dort jamais : c'est l'appelant
// qui decide d'attendre (serveur, sleep_until) ou de verifier a chaque frame (client, isTickDue)
class TickTimeManager
{
  public:
    using Clock = std::chrono::steady_clock;

    TickTimeManager();

    void start();
    void advance();

    Clock::time_point getNextTickTime() const;
    bool isTickDue() const;
    std::uint64_t getCurrentTick() const;

  private:
    Clock::time_point _startTime;
    std::uint64_t _currentTick = 0;
};
