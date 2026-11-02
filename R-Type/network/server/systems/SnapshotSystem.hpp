/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SnapshotSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"
#include <cstdint>

// Broadcasts the world once per server tick
class SnapshotSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    bool _sentOnce = false;
    std::uint32_t _lastTick = 0;
};
