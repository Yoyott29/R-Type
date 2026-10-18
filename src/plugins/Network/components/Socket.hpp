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
#include <string>

struct Socket {
    asio::io_context io_context;
    asio::ip::udp::socket socket{io_context};
    std::string serverAddress;
    uint16_t serverPort = 0;
    bool connected = false;
    uint8_t playerId = 0;
    uint32_t tick = 0;
    uint32_t lastSnapshotTick = 0;
    std::chrono::steady_clock::time_point lastConnectAttempt{};
};
