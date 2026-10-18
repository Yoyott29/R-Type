/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** main
*/

#include "server.hpp"
#include <cctype>
#include <charconv>
#include <cstring>
#include <exception>
#include <iostream>
#include <optional>
#include <string>

static std::optional<uint16_t> parsePort(const char *arg)
{
    unsigned int port = 0;
    const char *end = arg + std::strlen(arg);
    auto [ptr, error] = std::from_chars(arg, end, port);

    if (error != std::errc() || ptr != end || port == 0 || port > 65535)
        return std::nullopt;
    return static_cast<uint16_t>(port);
}

static std::optional<std::string> parseIp(const char *arg)
{
    int segments = 0;
    const char *ptr = arg;

    while (*ptr) {
        if (*ptr == '.') {
            segments++;
        } else if (!std::isdigit(*ptr)) {
            return std::nullopt;
        }
        ptr++;
    }
    if (segments != 3) {
        return std::nullopt;
    }
    return std::string(arg);
}

static int check_args(int argc, char **argv, std::string &ip, uint16_t &port)
{
    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " [ip] [port]" << std::endl;
        return 84;
    }
    if (argc == 2) {
        auto parsedPort = parsePort(argv[1]);
        if (!parsedPort.has_value()) {
            std::cerr << "Invalid port: " << argv[1] << std::endl;
            return 84;
        }
        port = *parsedPort;
    }
    if (argc == 3) {
        auto parsedIp = parseIp(argv[1]);
        if (!parsedIp.has_value()) {
            std::cerr << "Invalid ip: " << argv[1] << std::endl;
            return 84;
        }
        auto parsedPort = parsePort(argv[2]);
        if (!parsedPort.has_value()) {
            std::cerr << "Invalid port: " << argv[2] << std::endl;
            return 84;
        }
        ip = *parsedIp;
        port = *parsedPort;
    }
    return 0;
}

int main(int argc, char **argv)
{
    uint16_t port = rtype_protocol::DEFAULT_PORT;
    std::string ip = rtype_protocol::DEFAULT_ADDRESS;

    if (check_args(argc, argv, ip, port) == 84)
        return 84;
    try {
        rtype_server::Server server(port, ip);
        server.run();
    } catch (std::exception const &error) {
        std::cerr << "Fatal error: " << error.what() << std::endl;
        return 84;
    }
    return 0;
}
