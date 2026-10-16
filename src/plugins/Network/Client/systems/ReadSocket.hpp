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
#include "Sprite.hpp"
#include "rtype_protocol.hpp"
#include <array>
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

        scene.putAllEntitysInDestroyQueue();
        scene.flushDestroyQueue();
        rtype_protocol::Packet packet;
        asio::error_code ec;
        while (socket.socket.is_open()) {
            std::size_t len = socket.socket.receive(asio::buffer(packet), 0, ec);
            if (ec == asio::error::would_block)
                return;
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
    // temporary visuals, indexed by EntType, until real sprites are in place
    static inline std::array<Sprite, 4> const typeSprites = {{
        {BoxShape{50.0f, 35.0f}, Color{0, 228, 48, 255}},   // PLAYER
        {BoxShape{50.0f, 35.0f}, Color{228, 48, 0, 255}},   // ENEMY
        {CircleShape{5.0f}, Color{253, 249, 0, 255}},       // BULLET
        {BoxShape{60.0f, 60.0f}, Color{130, 130, 130, 255}} // OBSTACLE
    }};

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
            socket.tick = rtype_protocol::readValue<uint32_t>(&packet[1]);
            break;
        case rtype_protocol::SNAPSHOT: {
            constexpr std::size_t headerSize =
                sizeof(rtype_protocol::MessageType) + sizeof(rtype_protocol::SnapshotHeader);
            if (len < headerSize)
                return;
            {
                uint32_t tick = rtype_protocol::readValue<uint32_t>(&packet[1]);
                if (tick > socket.lastSnapshotTick)
                    socket.lastSnapshotTick = tick;
            }

            std::cout << "Processing snapshot with "
                      << (len - sizeof(rtype_protocol::MessageType) -
                          sizeof(rtype_protocol::SnapshotHeader)) /
                             sizeof(rtype_protocol::EntitySnapshot)
                      << " entities\n";
            for (std::size_t i = headerSize; i + sizeof(rtype_protocol::EntitySnapshot) <= len;
                 i += sizeof(rtype_protocol::EntitySnapshot)) {
                rtype_protocol::EntitySnapshot entity;
                entity.pos.x = rtype_protocol::readValue<float>(&packet[i]);
                entity.pos.y = rtype_protocol::readValue<float>(&packet[i + 4]);
                entity.type.type = static_cast<EntType>(packet[i + 2]);
                Entity e = scene.createEntity();
                scene.addComponent(e, entity.pos);
                scene.addComponent(e, entity.type);
                std::cout << "Entity: pos(" << entity.pos.x << ", " << entity.pos.y << "), type("
                          << static_cast<int>(entity.type.type) << ")\n";
                if (entity.type.type < typeSprites.size())
                    scene.addComponent(e, typeSprites[entity.type.type]);
            }
            break;
        }
        case rtype_protocol::DELTA:
            // Entity payload format is not specified yet: only the tick is acknowledged
            if (len < 5)
                return;
            {
                uint32_t tick = rtype_protocol::readValue<uint32_t>(&packet[1]);
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
