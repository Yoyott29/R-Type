/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** protocol
*/

// Format des paquets UDP, partage entre le client et le serveur

#pragma once
#include <cstdint>

namespace rtype_protocol
{

constexpr uint8_t PROTOCOL_VERSION = 1;

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

constexpr uint8_t INPUT_UNUSED_BITS = 0xE0; // doivent valoir 0

enum RejectReason : uint8_t {
    SERVER_FULL = 0x01,
    WRONG_VERSION = 0x02,
    GAME_ALREADY_STARTED = 0x03,
};

} // namespace rtype_protocol
