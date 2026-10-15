/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ReadSocket
*/

#pragma once
#include "AppAlive.hpp"
#include "Scene.hpp"
#include "Socket.hpp"
#include "rtype_protocol.hpp"
#include <cstddef>
#include <iostream>

class ReadSocket : public System
{
  public:
    void update(Scene &scene) override
    {
        Socket &socket = scene.getResource<Socket>();
        if (!socket.socket.is_open())
            return;

        // Drain every packet received since the last frame without blocking the loop
        rtype_protocol::Packet packet;
        asio::error_code ec;
        while (socket.socket.is_open()) {
            std::size_t len = socket.socket.receive(asio::buffer(packet), 0, ec);
            if (ec == asio::error::would_block)
                return;
            // ICMP answer to an earlier send: the server is not up yet, TryConnect retries
            if (ec == asio::error::connection_refused)
                return;
            if (ec) {
                std::cerr << "ReadSocket: " << ec.message() << "\n";
                return;
            }
            handlePacket(scene, socket, packet, len);
        }
    }

  private:
    static void handlePacket(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet,
                             std::size_t len)
    {
        if (len == 0)
            return;

        switch (packet[0]) {
        case rtype_protocol::CONNECT_ACCEPT:
            if (len < 2)
                return;
            socket.connected = true;
            socket.playerId = packet[1];
            break;
        case rtype_protocol::CONNECT_REJECT:
            if (len < 2)
                return;
            std::cerr << "Connection rejected by the server: " << rejectReason(packet[1]) << "\n";
            scene.getResource<AppAlive>().alive = false;
            break;
        case rtype_protocol::GAME_STARTED:
            if (len < 5)
                return;
            socket.tick = rtype_protocol::readU32(&packet[1]);
            break;
        case rtype_protocol::SNAPSHOT:
        case rtype_protocol::DELTA:
            // Entity payload format is not specified yet: only the tick is acknowledged
            if (len < 5)
                return;
            {
                uint32_t tick = rtype_protocol::readU32(&packet[1]);
                if (tick > socket.lastSnapshotTick)
                    socket.lastSnapshotTick = tick;
            }
            break;
        default:
            break;
        }
    }

    static char const *rejectReason(uint8_t reason)
    {
        switch (reason) {
        case rtype_protocol::SERVER_FULL:
            return "server full";
        case rtype_protocol::WRONG_VERSION:
            return "wrong protocol version";
        case rtype_protocol::GAME_ALREADY_STARTED:
            return "game already started";
        default:
            return "unknown reason";
        }
    }
};
