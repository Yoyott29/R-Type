/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** server
*/

#include "server.hpp"
#include "rtype_protocol.hpp"
#include <thread>

namespace rtype_server
{
Server::Server(uint16_t port, const std::string &ip)
    : _network(port, ip, _playerInputs, _running), _gameLoop(_network, _running)
{
    for (uint8_t id = 0; id < rtype_protocol::MAX_PLAYERS; id++)
        _playerInputs.try_emplace(id);
}

void Server::run()
{
    _running = true;
    std::thread networkThread(&Network::receiveLoop, &_network);
    try {
        _gameLoop.run();
    } catch (...) {
        _network.stop();
        networkThread.join();
        throw;
    }
    networkThread.join();
}
} // namespace rtype_server
