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
Server::Server(uint16_t port)
    : _network(port, _playerInputs, _running), _gameLoop(_network, _running)
{
    for (uint8_t id = 0; id < rtype_protocol::MAX_PLAYERS; id++)
        _playerInputs.try_emplace(id);
}

void Server::run()
{
    _running = true;
    std::thread networkThread(&Network::receiveLoop, &_network);
    _gameLoop.run();
    networkThread.join();

    networkThread = std::thread(&Network::sendSnapshot, &_network, 0,
                                std::vector<rtype_protocol::EntitySnapshot>());
    networkThread.join();
}
} // namespace rtype_server
