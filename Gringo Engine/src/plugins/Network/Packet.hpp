/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Packet
*/

#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

inline const std::uint16_t PROTOCOL_ID = 0x4752;
inline constexpr std::uint16_t RESERVED_TYPE_MIN = 0xFF00;
inline const std::size_t MAX_DATAGRAM_SIZE = 1200;
inline const std::size_t PACKET_HEADER_SIZE = 5;
inline const std::size_t MESSAGE_HEADER_SIZE = 4;
inline const std::size_t MAX_PAYLOAD = MAX_DATAGRAM_SIZE - PACKET_HEADER_SIZE - MESSAGE_HEADER_SIZE;

enum class PacketKind : std::uint8_t {
    Data,
    ConnectRequest,
    ConnectAccepted,
    ConnectRejected,
    KeepAlive,
    Disconnect
};

enum class RejectReason : std::uint8_t { Full, VersionMismatch };

// Message types the plugin keeps for itself; they never reach Events<NetMessage>
enum class ReservedType : std::uint16_t {
    EntityState = 0xFF01,
    Despawn = 0xFF02,
    InputState = 0xFF03,
    PeerLeft = 0xFF04
};

struct WireMessage {
    std::uint16_t type;
    std::vector<std::byte> payload;
};

struct Packet {
    PacketKind kind;
    std::uint16_t seq = 0;
    std::uint16_t version = 0;                // ConnectRequest only
    RejectReason reason = RejectReason::Full; // ConnectRejected only
    std::uint32_t peerId = 0;                 // ConnectAccepted only
    std::vector<WireMessage> messages;        // Data only
};

std::size_t encodedSize(WireMessage const &message);
std::vector<std::byte> writePacket(Packet const &packet);
std::optional<Packet> parsePacket(std::span<const std::byte> datagram);
std::vector<std::vector<std::size_t>> packMessages(std::span<WireMessage const> messages);
