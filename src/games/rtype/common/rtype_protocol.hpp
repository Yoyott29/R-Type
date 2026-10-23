/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** protocol
*/

// Constants shared by the client and the server

#pragma once
#include <cstdint>

namespace rtype_protocol
{
constexpr uint8_t MAX_PLAYERS = 4;
constexpr uint16_t DEFAULT_PORT = 4242;
constexpr const char *DEFAULT_ADDRESS = "127.0.0.1";

enum InputBits : uint8_t {
    UP = 1 << 0,     // 0b00000001
    DOWN = 1 << 1,   // 0b00000010
    LEFT = 1 << 2,   // 0b00000100
    RIGHT = 1 << 3,  // 0b00001000
    CHARGE = 1 << 4, // 0b00010000
};

constexpr uint8_t INPUT_UNUSED_BITS = 0xE0; // must be 0

constexpr uint8_t NO_PLAYER = 0xFF;
constexpr uint8_t DEFAULT_VARIANT = 0;
constexpr uint8_t ENEMY_VARIANT_2 = 1;
constexpr uint8_t BOX_SHAPE = 0;
constexpr uint8_t CIRCLE_SHAPE = 1;

} // namespace rtype_protocol
