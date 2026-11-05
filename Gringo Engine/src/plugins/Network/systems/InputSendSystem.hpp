/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** InputSendSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"
#include <cstdint>
#include <optional>

// Client, PostUpdate before NetSendSystem: while connected, sends the ActionInput bits (0 without
// the resource) to the server once per GameClock tick, however many frames that tick spans
class InputSendSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    std::optional<std::uint64_t> _lastSent;
};
