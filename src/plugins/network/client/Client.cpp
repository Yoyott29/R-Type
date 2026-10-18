/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Client
*/

#include "Client.hpp"
#include "RTypeProtocol.hpp"
#include "ReadSocket.hpp"
#include "SendInput.hpp"
#include "TryConnect.hpp"
#include <array>
#include <iostream>

Client::Client(const std::string &serverAddress, uint16_t serverPort)
    : _serverAddress(serverAddress), _serverPort(serverPort)
{
}

void Client::build(Application &app)
{
    Socket &socket = app._scene.addResource<Socket>();
    socket.serverAddress = _serverAddress;
    socket.serverPort = _serverPort;

    // Connect and read the server's packets before the frame's logic runs
    app._scene.registerSystem<TryConnect>(PrepUpdate);
    app._scene.registerSystem<SendInput>(StateTransition);
    app._scene.registerSystem<ReadSocket>(StateTransition);
}

void Client::cleanup(Application &app)
{
    Socket &socket = app._scene.getResource<Socket>();
    if (!socket.socket.is_open())
        return;

    if (socket.connected) {
        asio::error_code ec;
        std::array<uint8_t, 1> packet = {RTypeProtocol::DISCONNECT};
        socket.socket.send(asio::buffer(packet), 0, ec);
        if (ec)
            std::cerr << "Client: " << ec.message() << "\n";
    }
    socket.connected = false;
    socket.socket.close();
}
