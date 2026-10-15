/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** TryConnect
*/

#pragma once
#include "Scene.hpp"
#include "Socket.hpp"
#include "rtype_protocol.hpp"
#include <array>
#include <chrono>
#include <iostream>

class TryConnect : public System
{
  public:
    void update(Scene &scene) override
    {
        Socket &socket = scene.getResource<Socket>();

        if (socket.connected) {
            scene.removeSystem<TryConnect>();
            return;
        }

        auto now = std::chrono::steady_clock::now();
        if (now - socket.lastConnectAttempt < CONNECT_RETRY_DELAY)
            return;
        socket.lastConnectAttempt = now;

        asio::error_code ec;
        if (!socket.socket.is_open() && !openSocket(socket, ec)) {
            std::cerr << "TryConnect: " << ec.message() << "\n";
            return;
        }

        std::array<uint8_t, 2> packet = {rtype_protocol::CONNECT, rtype_protocol::PROTOCOL_VERSION};
        socket.socket.send(asio::buffer(packet), 0, ec);
        if (ec)
            std::cerr << "TryConnect: " << ec.message() << "\n";
    }

  private:
    static constexpr std::chrono::seconds CONNECT_RETRY_DELAY{1};

    static bool openSocket(Socket &socket, asio::error_code &ec)
    {
        asio::ip::udp::endpoint server(asio::ip::make_address(DEFAULT_ADDRESS, ec), DEFAULT_PORT);

        if (ec)
            return false;
        socket.socket.connect(server, ec);
        if (!ec)
            socket.socket.non_blocking(true, ec);
        if (ec) {
            asio::error_code ignored;
            socket.socket.close(ignored);
            return false;
        }
        return true;
    }
};
