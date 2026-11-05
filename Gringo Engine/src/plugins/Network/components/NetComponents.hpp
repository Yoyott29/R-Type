/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetComponents
*/

#pragma once
#include <cstdint>

// On a client's mirror of a server entity
struct Mirror {
    std::uint32_t netId;
    std::uint32_t lastSeen;
};

// On a server entity that is replicated
struct NetId {
    std::uint32_t value;
};
