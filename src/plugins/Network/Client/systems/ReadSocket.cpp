/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ReadSocket
*/

#include "ReadSocket.hpp"
#include "AppAlive.hpp"
#include "Sprite.hpp"
#include <array>
#include <iostream>

namespace
{
// temporary visuals, indexed by EntType, until real sprites are in place
std::array<Sprite, 4> const typeSprites = {{
    {BoxShape{50.0f, 35.0f}, Color{0, 228, 48, 255}},   // PLAYER
    {BoxShape{50.0f, 35.0f}, Color{228, 48, 0, 255}},   // ENEMY
    {CircleShape{5.0f}, Color{253, 249, 0, 255}},       // BULLET
    {BoxShape{60.0f, 60.0f}, Color{130, 130, 130, 255}} // OBSTACLE
}};
} // namespace

void ReadSocket::update(Scene &scene)
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

void ReadSocket::handlePacket(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet,
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
    case rtype_protocol::PLAYER_LEFT:
        if (len < 2)
            return;
        if (socket.connected && packet[1] == socket.playerId) {
            std::cerr << "Player eliminated by the server\n";
            scene.getResource<AppAlive>().alive = false;
        }
        break;
    case rtype_protocol::GAME_STARTED:
        if (len < 5)
            return;
        socket.tick = rtype_protocol::readValue<uint32_t>(&packet[1]);
        break;
    case rtype_protocol::SNAPSHOT: {
        constexpr std::size_t headerSize = rtype_protocol::SNAPSHOT_HEADER_SIZE;
        if (len < headerSize)
            return;
        {
            uint32_t tick = rtype_protocol::readValue<uint32_t>(&packet[1]);
            if (tick > socket.lastSnapshotTick)
                socket.lastSnapshotTick = tick;
        }

        std::cout << "Processing snapshot with " << (len - headerSize) / rtype_protocol::ENTITY_SIZE
                  << " entities\n";
        for (std::size_t i = headerSize; i + rtype_protocol::ENTITY_SIZE <= len;
             i += rtype_protocol::ENTITY_SIZE) {
            rtype_protocol::EntitySnapshot entity;
            uint8_t type = packet[i + 8];
            entity.pos.x = rtype_protocol::readValue<float>(&packet[i]);
            entity.pos.y = rtype_protocol::readValue<float>(&packet[i + 4]);
            if (type >= typeSprites.size()) {
                std::cout << "Entity skipped: unknown type " << static_cast<int>(type) << "\n";
                continue;
            }
            entity.type.type = static_cast<EntType>(type);
            Entity e = scene.createEntity();
            scene.addComponent(e, entity.pos);
            scene.addComponent(e, entity.type);
            std::cout << "Entity: pos(" << entity.pos.x << ", " << entity.pos.y << "), type("
                      << static_cast<int>(entity.type.type) << ")\n";
            scene.addComponent(e, typeSprites[type]);
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

char const *ReadSocket::rejectReason(uint8_t reason)
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
