/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** main
*/

#include "Application.hpp"
#include "NetworkConfig.hpp"
#include "PluginLoader.hpp"
#include "rtype_protocol.hpp"
#include <array>
#include <asio.hpp>
#include <charconv>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>

// Loaded in this order from the plugins folder next to the executable
static constexpr std::array<char const *, 4> PLUGINS = {"RayLibRenderer", "Physics", "Network",
                                                        "RTypeClient"};

static std::optional<uint16_t> parsePort(const char *arg)
{
    unsigned int port = 0;
    const char *end = arg + std::strlen(arg);
    auto [ptr, error] = std::from_chars(arg, end, port);

    if (error != std::errc() || ptr != end || port == 0 || port > 65535)
        return std::nullopt;
    return static_cast<uint16_t>(port);
}

static bool isValidIp(const char *arg)
{
    asio::error_code error;
    asio::ip::make_address_v4(arg, error);
    return !error;
}

static int checkArgs(int argc, char **argv, std::string &ip, uint16_t &port)
{
    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " [ip] [port]\n";
        return 84;
    }
    if (argc == 3) {
        if (!isValidIp(argv[1])) {
            std::cerr << "Invalid ip: " << argv[1] << "\n";
            return 84;
        }
        ip = argv[1];
    }
    if (argc >= 2) {
        auto parsedPort = parsePort(argv[argc - 1]);
        if (!parsedPort.has_value()) {
            std::cerr << "Invalid port: " << argv[argc - 1] << "\n";
            return 84;
        }
        port = *parsedPort;
    }
    return 0;
}

int main(int argc, char **argv)
{
    std::string ip = rtype_protocol::DEFAULT_ADDRESS;
    uint16_t port = rtype_protocol::DEFAULT_PORT;

    if (checkArgs(argc, argv, ip, port) == 84)
        return 84;
    // Declared outside the try: the plugins' code must stay loaded while their errors are caught
    PluginLoader loader;
    try {
        Application application;
        application._scene.addResource<NetworkConfig>(NetworkConfig::client(ip, port));
        for (char const *plugin : PLUGINS)
            application.addPlugin(loader.load(plugin));
        std::cout << "Connecting to " << ip << ":" << port << "\n" << std::flush;
        application.run();
    } catch (std::exception const &error) {
        std::cerr << "Fatal error: " << error.what() << "\n";
        return 84;
    }
    return 0;
}
