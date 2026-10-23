/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ClientSession
*/

#pragma once
#include <cstdint>
#include <optional>
#include <string>

struct ClientSession {
    std::string server;
    std::optional<std::uint8_t> playerId;
    std::uint32_t inputTick = 0;
    std::optional<std::uint32_t> displayedTick;
};
