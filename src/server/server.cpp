/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** server
*/

#include "server.hpp"
#include <thread>

namespace rtype_server
{
Server::Server(uint16_t port) : _network(port, _inputs, _running), _gameLoop(_inputs, _running) {}

void Server::run()
{
    _running = true;
    std::thread networkThread(&Network::receiveLoop, &_network);
    _gameLoop.run();
    networkThread.join();
}
} // namespace rtype_server
