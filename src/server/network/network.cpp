/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** network
*/

#include "network/network.hpp"
#include "common/protocol.hpp"
#include <iostream>
#include <netinet/in.h>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>

namespace rtype_server
{
Network::Network(uint16_t port, InputQueue &inputs, std::atomic<bool> &running)
    : _inputs(inputs), _running(running)
{
    _socket = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);
    bind(_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
}

Network::~Network()
{
    if (_socket >= 0)
        close(_socket);
}

// Thread reseau : recoit les paquets en continu et range les inputs dans la queue
void Network::receiveLoop()
{
    char buffer[1024];
    sockaddr_in clientAddress;
    socklen_t clientAddressLength;

    while (_running) {
        clientAddressLength = sizeof(clientAddress);
        ssize_t n = recvfrom(_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddress,
                             &clientAddressLength);
        if (n < 0)
            break;
        std::string_view message(buffer, n);
        if (message == "exit\n")
            break;
        if (n == 2 && buffer[0] == rtype_protocol::INPUT) {
            _inputs.push(static_cast<uint8_t>(buffer[1]));
            continue;
        }
        // std::cout << "Received a packet from a client : " << message;
    }
    _running = false;
}
} // namespace rtype_server
