/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** main
*/

#include "server.hpp"
#include <charconv>
#include <cstring>
#include <exception>
#include <iostream>
#include <optional>

static std::optional<uint16_t> parsePort(const char *arg)
{
    unsigned int port = 0;
    const char *end = arg + std::strlen(arg);
    auto [ptr, error] = std::from_chars(arg, end, port);

    if (error != std::errc() || ptr != end || port == 0 || port > 65535)
        return std::nullopt;
    return static_cast<uint16_t>(port);
}

int main(int argc, char **argv)
{
    uint16_t port = rtype_server::Server::DEFAULT_PORT;

    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [port]" << std::endl;
        return 84;
    }
    if (argc == 2) {
        std::optional<uint16_t> parsed = parsePort(argv[1]);
        if (!parsed) {
            std::cerr << "Invalid port: " << argv[1] << std::endl
                      << "Usage: " << argv[0] << " [port]" << std::endl;
            return 84;
        }
        port = *parsed;
    }
    try {
        rtype_server::Server server(port);
        server.run();
    } catch (std::exception const &error) {
        std::cerr << "Fatal error: " << error.what() << std::endl;
        return 84;
    }
    return 0;
}
