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

// Octet 0 de chaque paquet : le type de message
enum MessageType : uint8_t {
    INPUT = 0x01, // [INPUT, InputBits]
};

// Octet 1 d'un paquet INPUT : un bit par touche enfoncee
enum InputBits : uint8_t {
    UP = 1 << 0,     // 0b00000001
    DOWN = 1 << 1,   // 0b00000010
    LEFT = 1 << 2,   // 0b00000100
    RIGHT = 1 << 3,  // 0b00001000
    CHARGE = 1 << 4, // 0b00010000
};

} // namespace rtype_protocol
