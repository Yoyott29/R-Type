/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ReplicationSystem
*/

#pragma once
#include "Messages.hpp"
#include "NetLog.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Ticks per snapshot: one every other GameClock tick (30 Hz)
inline constexpr std::uint64_t SNAPSHOT_TICK_DIVISOR = 2;
// How many snapshots repeat a despawn
inline constexpr int DESPAWN_REPEATS = 3;

// Server: gives every Networked{Replicated} entity a NetId, then once per snapshot broadcasts one
// EntityState per entity and the Despawn of the netIds that left
class ReplicationSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    void assignNetIds(Scene &scene);
    // The payload, or nullopt (logged once per netId) if it does not fit one message
    std::optional<std::vector<std::byte>> tryEncode(EntityStateMessage const &message);

    std::uint32_t _nextNetId = 1;
    std::optional<std::uint64_t> _lastSnapshot;
    // The netIds of the last snapshot, to find the ones that left
    std::unordered_set<std::uint32_t> _previous;
    // Despawn countdown: netId -> snapshots that still repeat its despawn
    std::unordered_map<std::uint32_t, int> _despawns;
    LogOnce _log;
};
