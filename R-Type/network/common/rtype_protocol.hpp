/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** protocol
*/

// Constants shared by the client and the server

#pragma once
#include <string>
#include <cstdint>

namespace rtype_protocol
{
const uint8_t MAX_PLAYERS = 4;
const uint16_t DEFAULT_PORT = 4242;
inline const std::string DEFAULT_ADDRESS = "127.0.0.1";

enum InputBits : uint8_t {
    UP = 1 << 0,     // 0b00000001
    DOWN = 1 << 1,   // 0b00000010
    LEFT = 1 << 2,   // 0b00000100
    RIGHT = 1 << 3,  // 0b00001000
    CHARGE = 1 << 4, // 0b00010000
};

const uint8_t INPUT_UNUSED_BITS = 0xE0; // must be 0

const uint8_t NO_PLAYER = 0xFF;
const uint8_t DEFAULT_VARIANT = 0;
const uint8_t ENEMY_VARIANT_2 = 1;

} // namespace rtype_protocol
