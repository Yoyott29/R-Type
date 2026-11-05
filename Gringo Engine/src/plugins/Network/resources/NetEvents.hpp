/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetEvents
*/

#pragma once
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

using PeerId = std::uint32_t;

inline constexpr PeerId NO_PEER = std::numeric_limits<PeerId>::max();

enum class DisconnectReason : std::uint8_t {
    Timeout,
    Remote,
    ConnectFailed,
    Full,
    VersionMismatch
};

struct PeerConnected {
    PeerId peer;
};

struct PeerDisconnected {
    PeerId peer;
    DisconnectReason reason;
};

// A remote peer that left, announced by the server to the other clients
struct PeerLeft {
    PeerId peer;
    DisconnectReason reason;
};

struct NetMessage {
    PeerId peer;
    std::uint16_t type;
    std::vector<std::byte> payload;
};
