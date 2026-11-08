/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** TickTimeManager
*/

#include "TickTimeManager.hpp"

TickTimeManager::TickTimeManager() { start(); }

// A appeler juste avant la boucle, pour ne pas compter le temps d'initialisation
void TickTimeManager::start()
{
    _startTime = Clock::now();
    _currentTick = 0;
}

// A appeler apres chaque tick execute
void TickTimeManager::advance() { _currentTick++; }

// Apres un gros retard : abandonne les ticks en retard, le prochain tick est dans un tick
void TickTimeManager::resync()
{
    _startTime = std::chrono::time_point_cast<Clock::duration>(
        Clock::now() - static_cast<std::int64_t>(_currentTick) * FIXED_TICK_DURATION);
}

// Calcule depuis le depart : aucune erreur d'arrondi ne s'accumule
TickTimeManager::Clock::time_point TickTimeManager::getNextTickTime() const
{
    return std::chrono::time_point_cast<Clock::duration>(
        _startTime + static_cast<std::int64_t>(_currentTick + 1) * FIXED_TICK_DURATION);
}

bool TickTimeManager::isTickDue() const { return Clock::now() >= getNextTickTime(); }

std::uint64_t TickTimeManager::getCurrentTick() const { return _currentTick; }
