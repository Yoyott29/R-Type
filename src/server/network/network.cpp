/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** network
*/

#include "network/network.hpp"
#include "rtype_protocol.hpp"
#include <asio.hpp>
#include <iostream>
#include <string_view>

namespace rtype_server
{
Network::Network(uint16_t port, std::map<uint8_t, InputQueue> &inputs, std::atomic<bool> &running)
    : _endpoint(asio::ip::udp::v4(), port), _socketAsio(_ioContext, _endpoint),
      _playerInputs(inputs), _running(running)
{
    _running = true;
    std::cout << "Server listening on port : " << port << std::endl;
}

Network::~Network()
{
    if (_socketAsio.is_open()) {
        std::cout << "Closing server" << std::endl;
        _socketAsio.close();
    }
}

const std::map<asio::ip::udp::endpoint, uint8_t> &Network::getPlayers() const { return _players; }

std::map<uint8_t, InputQueue> &Network::getPlayerInputs() { return _playerInputs; }

// Thread reseau : recoit les paquets en continu et range les inputs dans la queue
void Network::receiveLoop()
{
    char buffer[1024];
    asio::ip::udp::endpoint sender;
    asio::error_code error;

    while (_running) {
        std::size_t n = _socketAsio.receive_from(asio::buffer(buffer), sender, 0, error);
        if (error)
            break;
        std::string_view message(buffer, n);

        if (message == "exit\n")
            break;
        if (n == 0)
            continue;
        uint8_t type = static_cast<uint8_t>(buffer[0]);
        if (type == rtype_protocol::CONNECT) {
            handleConnect(buffer, n, sender);
            continue;
        }
        if (_players.find(sender) == _players.end()) {
            reply(sender, rtype_protocol::CONNECT_REJECT, rtype_protocol::NOT_CONNECTED);
            continue;
        }
        if (n == 2 && type == rtype_protocol::INPUT) {
            uint8_t playerId = _players[sender];
            _playerInputs.at(playerId).push(static_cast<uint8_t>(buffer[1]));
            continue;
        }
    }
    _running = false;
}

void Network::handleConnect(const char *packet, std::size_t size,
                            const asio::ip::udp::endpoint &sender)
{
    if (size != 2)
        return;
    if (static_cast<uint8_t>(packet[1]) != rtype_protocol::PROTOCOL_VERSION) {
        reply(sender, rtype_protocol::CONNECT_REJECT, rtype_protocol::WRONG_VERSION);
        return;
    }
    auto player = _players.find(sender);
    if (player != _players.end()) {
        reply(sender, rtype_protocol::CONNECT_ACCEPT, player->second);
        return;
    }
    for (uint8_t id = 0; id < rtype_protocol::MAX_PLAYERS; id++) {
        bool taken = false;
        for (auto const &[endpoint, playerId] : _players)
            if (playerId == id)
                taken = true;
        if (!taken) {
            _players[sender] = id;
            std::cout << "Player " << static_cast<int>(id)
                      << " connected: " << sender.address().to_string() << ":" << sender.port()
                      << std::endl;
            reply(sender, rtype_protocol::CONNECT_ACCEPT, id);
            return;
        }
    }
    reply(sender, rtype_protocol::CONNECT_REJECT, rtype_protocol::SERVER_FULL);
}

void Network::reply(const asio::ip::udp::endpoint &target, uint8_t type, uint8_t value)
{
    const uint8_t packet[] = {type, value};
    asio::error_code error;

    _socketAsio.send_to(asio::buffer(packet), target, 0, error);
}
} // namespace rtype_server
