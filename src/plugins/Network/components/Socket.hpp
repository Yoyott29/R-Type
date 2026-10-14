/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Socket
*/

#pragma once
#include <asio.hpp>
#include <chrono>
#include <cstdint>
#define DEFAULT_PORT 4242
#define DEFAULT_ADDRESS "127.0.0.1"

struct Socket {
    asio::io_context io_context;
    asio::ip::udp::socket socket{io_context};
    bool connected = false;
    uint8_t playerId = 0;
    uint32_t tick = 0;
    uint32_t lastSnapshotTick = 0;
    std::chrono::steady_clock::time_point lastConnectAttempt{};
};
