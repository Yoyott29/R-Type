/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** main
*/

#include "Application.hpp"
#include "NetworkConfig.hpp"
#include "rtype_protocol.hpp"
#include <asio.hpp>
#include <array>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// Loaded in this order from the plugins folder next to the executable
static const std::array<std::string, 5> PLUGINS = {"2D", "InputHandler", "Physics", "Network", "RTypeClient"};

static std::optional<uint16_t> parsePort(const std::string &arg)
{
    // Digits only, so std::stoul cannot throw nor accept "-1" or " 42"
    if (arg.empty() || arg.size() > 5 || arg.find_first_not_of("0123456789") != std::string::npos)
        return std::nullopt;
    const unsigned long port = std::stoul(arg);
    if (port == 0 || port > 65535)
        return std::nullopt;
    return static_cast<uint16_t>(port);
}

static bool isValidIp(const std::string &arg)
{
    asio::error_code error;
    asio::ip::make_address_v4(arg, error);
    return !error;
}

// arguments: the command line, the program name first
static int checkArgs(const std::vector<std::string> &arguments, std::string &ip, uint16_t &port)
{
    if (arguments.size() > 3) {
        std::cerr << "Usage: " << arguments[0] << " [ip] [port]\n";
        return 84;
    }
    if (arguments.size() == 3) {
        if (!isValidIp(arguments[1])) {
            std::cerr << "Invalid ip: " << arguments[1] << "\n";
            return 84;
        }
        ip = arguments[1];
    }
    if (arguments.size() >= 2) {
        auto parsedPort = parsePort(arguments.back());
        if (!parsedPort.has_value()) {
            std::cerr << "Invalid port: " << arguments.back() << "\n";
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

    const std::vector<std::string> arguments(argv, argv + argc);

    if (checkArgs(arguments, ip, port) == 84)
        return 84;
    try {
        Application application;
        application._scene.addResource<NetworkConfig>(NetworkConfig::client(ip, port));
        application.setRenderer("RaylibRenderer", "R-Type");
        for (const std::string &plugin : PLUGINS)
            application.addPlugin(plugin);
        std::cout << "Connecting to " << ip << ":" << port << "\n" << std::flush;
        application.run();
    } catch (std::exception const &error) {
        std::cerr << "Fatal error: " << error.what() << "\n";
        return 84;
    }
    return 0;
}
