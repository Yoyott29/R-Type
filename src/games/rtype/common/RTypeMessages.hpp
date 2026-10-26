/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RTypeMessages
*/

#pragma once
#include "Type.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

enum class RTypeMessage : std::uint16_t { Welcome = 1, Input = 2, Snapshot = 3 };

struct WelcomeMessage {
    std::uint8_t playerId;
};

struct InputMessage {
    std::uint32_t tick;
    std::uint8_t buttons;
};

struct SnapshotEntity {
    float x;
    float y;
    EntType type;
    std::uint8_t playerId;
    std::uint8_t variant;

    bool operator==(SnapshotEntity const &) const = default;
};

struct SnapshotMessage {
    std::uint32_t tick;
    std::vector<SnapshotEntity> entities;
};

constexpr std::size_t SNAPSHOT_HEADER_BYTES = 6;         // tick u32, count u16
constexpr std::size_t SNAPSHOT_ENTITY_BYTES = 11;        // fields in declaration order, type as u8
constexpr std::size_t SNAPSHOT_ENTITIES_PER_CHUNK = 107; // (MAX_PAYLOAD 1191 - 6) / 11

std::vector<std::byte> encodeWelcome(WelcomeMessage message);
WelcomeMessage decodeWelcome(std::span<const std::byte> bytes);
std::vector<std::byte> encodeInput(InputMessage message);
InputMessage decodeInput(std::span<const std::byte> bytes);
std::vector<std::vector<std::byte>> encodeSnapshot(std::uint32_t tick,
                                                   std::span<const SnapshotEntity> entities);
SnapshotMessage decodeSnapshot(std::span<const std::byte> bytes);
