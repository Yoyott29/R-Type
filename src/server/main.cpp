/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** main
*/

#include "Server.hpp"
#include <asio.hpp>
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

static bool isValidIp(const char *arg)
{
    asio::error_code error;
    asio::ip::make_address_v4(arg, error);
    return !error;
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
        if (!isValidIp(argv[1])) {
            std::cerr << "Invalid ip: " << argv[1] << std::endl;
            return 84;
        }
        auto parsedPort = parsePort(argv[2]);
        if (!parsedPort.has_value()) {
            std::cerr << "Invalid port: " << argv[2] << std::endl;
            return 84;
        }
        ip = argv[1];
        port = *parsedPort;
    }
    return 0;
}

int main(int argc, char **argv)
{
    uint16_t port = RTypeProtocol::DEFAULT_PORT;
    std::string ip = RTypeProtocol::DEFAULT_ADDRESS;

    if (check_args(argc, argv, ip, port) == 84)
        return 84;
    try {
        RTypeServer::Server server(port, ip);
        server.run();
    } catch (std::exception const &error) {
        std::cerr << "Fatal error: " << error.what() << std::endl;
        return 84;
    }
    return 0;
}
