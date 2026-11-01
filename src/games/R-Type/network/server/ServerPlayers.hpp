/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ServerPlayers
*/

#pragma once
#include "Entity.hpp"
#include "NetEvents.hpp"
#include "Scene.hpp"
#include "rtype_protocol.hpp"
#include <cstdint>
#include <deque>
#include <optional>
#include <unordered_map>

struct ServerTick {
    std::uint32_t value = 0;
};

struct ServerPlayer {
    std::uint8_t slot;
    Entity entity;
    std::deque<std::uint8_t> inputs;
    float shootCooldown = 0.0f;
};

struct ServerPlayers {
    std::unordered_map<PeerId, ServerPlayer> byPeer;

    // Lowest slot in 0..MAX_PLAYERS-1 not used
    std::optional<std::uint8_t> freeSlot() const
    {
        for (std::uint8_t slot = 0; slot < rtype_protocol::MAX_PLAYERS; ++slot) {
            bool used = false;
            for (auto const &[peer, player] : byPeer)
                used = used || player.slot == slot;
            if (!used)
                return slot;
        }
        return std::nullopt;
    }

    std::optional<PeerId> peerOf(Entity entity) const
    {
        for (auto const &[peer, player] : byPeer)
            if (player.entity == entity)
                return peer;
        return std::nullopt;
    }
};

// Disconnects the peer, queues its ship for destruction, frees its slot; no-op if unknown
void eliminatePlayer(Scene &scene, PeerId peer);
