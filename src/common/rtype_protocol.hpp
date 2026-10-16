/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** protocol
*/

// Format des paquets UDP, partage entre le client et le serveur

#pragma once
#include "Position.hpp"
#include "Type.hpp"
#include <array>
#include <cstdint>

namespace rtype_protocol
{
using Packet = std::array<uint8_t, 1024>; // taille max d'un paquet UDP

constexpr uint8_t PROTOCOL_VERSION = 1;
constexpr uint8_t MAX_PLAYERS = 4;
constexpr uint16_t DEFAULT_PORT = 4242;
constexpr const char *DEFAULT_ADDRESS = "127.0.0.1";

// Octet 0 : bit 7 = sens (0 client -> serveur, 1 serveur -> client), bits 6-4 = categorie
enum MessageType : uint8_t {
    CONNECT = 0x10,        // [CONNECT, version]
    DISCONNECT = 0x11,     // [DISCONNECT]
    GAME_START = 0x20,     // [GAME_START]
    INPUT = 0x30,          // [INPUT, tick (u32), InputBits, lastSnapshotTick (u32)]
    CONNECT_ACCEPT = 0x90, // [CONNECT_ACCEPT, playerId]
    CONNECT_REJECT = 0x91, // [CONNECT_REJECT, RejectReason]
    PLAYER_LEFT = 0x92,    // [PLAYER_LEFT, playerId]
    GAME_STARTED = 0xA0,   // [GAME_STARTED, startTick (u32)]
    SNAPSHOT = 0xC0,       // [SNAPSHOT, tick (u32), count (u16), entites...]
    DELTA = 0xC1,          // [DELTA, tick (u32), baseTick (u32), count (u16), entites...]
};

constexpr uint8_t SERVER_TO_CLIENT_BIT = 0x80;

enum InputBits : uint8_t {
    UP = 1 << 0,     // 0b00000001
    DOWN = 1 << 1,   // 0b00000010
    LEFT = 1 << 2,   // 0b00000100
    RIGHT = 1 << 3,  // 0b00001000
    CHARGE = 1 << 4, // 0b00010000
};

struct InputPacket {
    uint32_t tick;
    uint8_t inputBits;
    uint32_t lastSnapshotTick;
};

constexpr uint8_t INPUT_UNUSED_BITS = 0xE0; // doivent valoir 0

enum RejectReason : uint8_t {
    SERVER_FULL = 0x01,
    WRONG_VERSION = 0x02,
    GAME_ALREADY_STARTED = 0x03,
    NOT_CONNECTED = 0x04,
};

// Les entiers multi-octets sont envoyes en big-endian
inline void writeU32(uint8_t *out, uint32_t value)
{
    out[0] = static_cast<uint8_t>(value >> 24);
    out[1] = static_cast<uint8_t>(value >> 16);
    out[2] = static_cast<uint8_t>(value >> 8);
    out[3] = static_cast<uint8_t>(value);
}

inline uint32_t readU32(const uint8_t *in)
{
    return (static_cast<uint32_t>(in[0]) << 24) | (static_cast<uint32_t>(in[1]) << 16) |
           (static_cast<uint32_t>(in[2]) << 8) | static_cast<uint32_t>(in[3]);
}

struct SnapshotHeader {
    uint32_t tick;
    uint16_t entityCount;
};

struct EntitySnapshot {
    Position pos;
    Type type;
};

} // namespace rtype_protocol
