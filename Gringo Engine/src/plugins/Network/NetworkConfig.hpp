/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** NetworkConfig
*/

#pragma once
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

using NetClock = std::chrono::steady_clock;
using TimePoint = NetClock::time_point;

enum class NetRole { Server, Client };

struct NetworkConfig {
    NetRole role = NetRole::Server;
    std::string host;
    std::uint16_t port = 0;
    std::uint16_t protocolVersion = 1;
    std::size_t maxPeers = 4;
    int connectAttempts = 25;
    std::chrono::milliseconds connectRetryInterval{200};
    std::chrono::milliseconds keepAliveInterval{250};
    std::chrono::milliseconds timeout{5000};
    int disconnectRedundancy = 3;
    bool logStats = false; // logs per-peer receive stats to stderr every second

    static NetworkConfig server(std::uint16_t port, std::string host = "0.0.0.0")
    {
        NetworkConfig config;
        config.role = NetRole::Server;
        config.host = std::move(host);
        config.port = port;
        return config;
    }

    static NetworkConfig client(std::string host, std::uint16_t port)
    {
        NetworkConfig config;
        config.role = NetRole::Client;
        config.host = std::move(host);
        config.port = port;
        return config;
    }
};
