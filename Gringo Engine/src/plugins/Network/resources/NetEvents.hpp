/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetEvents
*/

#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

using PeerId = std::uint32_t;

enum class DisconnectReason { Timeout, Remote, ConnectFailed, Full, VersionMismatch };

struct PeerConnected {
    PeerId peer;
};

struct PeerDisconnected {
    PeerId peer;
    DisconnectReason reason;
};

struct NetMessage {
    PeerId peer;
    std::uint16_t type;
    std::vector<std::byte> payload;
};
