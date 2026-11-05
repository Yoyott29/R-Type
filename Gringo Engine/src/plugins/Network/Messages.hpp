/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Messages
*/

#pragma once
#include "resources/NetEvents.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>

inline constexpr std::size_t MAX_PREFAB_NAME = 64;
inline constexpr std::size_t MAX_MIRRORS = 4096;

struct CodecBlock {
    std::uint32_t codecId;
    std::vector<std::byte> bytes;
};

// 0xFF01
struct EntityStateMessage {
    std::uint32_t tick;
    std::uint32_t netId;
    std::uint32_t owner;
    std::string prefab;
    std::vector<CodecBlock> blocks;

    // Throws NetError on a value that does not fit the wire form
    std::vector<std::byte> encode() const;
    // nullopt on a truncation, trailing bytes or a prefab over MAX_PREFAB_NAME
    static std::optional<EntityStateMessage> decode(std::span<const std::byte> payload);
};

// 0xFF02
struct DespawnMessage {
    std::uint32_t tick;
    std::vector<std::uint32_t> netIds;

    // One payload per MAX_PAYLOAD worth of ids; none for an empty list
    static std::vector<std::vector<std::byte>> encodeSplit(std::uint32_t tick,
                                                           std::span<const std::uint32_t> netIds);
    static std::optional<DespawnMessage> decode(std::span<const std::byte> payload);
};

// 0xFF03
struct InputStateMessage {
    std::uint32_t tick;
    std::uint32_t bits;

    std::vector<std::byte> encode() const;
    static std::optional<InputStateMessage> decode(std::span<const std::byte> payload);
};

// 0xFF04
struct PeerLeftMessage {
    PeerId peer;
    DisconnectReason reason;

    std::vector<std::byte> encode() const;
    // nullopt also on a reason above VersionMismatch
    static std::optional<PeerLeftMessage> decode(std::span<const std::byte> payload);
};
