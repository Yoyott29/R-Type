/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ReadSocket
*/

#include "ReadSocket.hpp"
#include "AppAlive.hpp"
#include "EngineError.hpp"
#include "Sprite.hpp"
#include <array>

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

    _snapshotParts.clear();
    rtype_protocol::Packet packet;
    asio::error_code ec;
    while (socket.socket.is_open()) {
        std::size_t len = socket.socket.receive(asio::buffer(packet), 0, ec);
        if (ec == asio::error::would_block)
            break;
        if (ec == asio::error::connection_refused)
            break;
        if (ec) {
            std::cerr << "ReadSocket: " << ec.message() << "\n";
            break;
        }
        handlePacket(scene, socket, packet, len);
    }
    applySnapshot(scene, socket);
}

void ReadSocket::applySnapshot(Scene &scene, Socket &socket)
{
    if (_snapshotParts.empty())
        return;
    if (_snapshotTick > _displayedTick) {
        scene.putAllEntitysInDestroyQueue();
        scene.flushDestroyQueue();
        _displayedTick = _snapshotTick;
    }
    for (auto const &[packet, len] : _snapshotParts)
        createEntities(scene, packet, len);
    if (_snapshotTick > socket.lastSnapshotTick)
        socket.lastSnapshotTick = _snapshotTick;
}

void ReadSocket::createEntities(Scene &scene, rtype_protocol::Packet const &packet, std::size_t len)
{
    constexpr std::size_t headerSize = rtype_protocol::SNAPSHOT_HEADER_SIZE;

    for (std::size_t i = headerSize; i + rtype_protocol::ENTITY_SIZE <= len;
         i += rtype_protocol::ENTITY_SIZE) {
        rtype_protocol::EntitySnapshot entity;
        uint8_t type = packet[i + 8];
        entity.pos.x = rtype_protocol::readValue<float>(&packet[i]);
        entity.pos.y = rtype_protocol::readValue<float>(&packet[i + 4]);
        if (type >= typeSprites.size()) {
            throw UnknownComponentType(std::to_string(type));
        }
        entity.type.type = static_cast<EntType>(type);
        Entity e = scene.createEntity();
        scene.addComponent(e, entity.pos);
        scene.addComponent(e, entity.type);
        scene.addComponent(e, typeSprites[type]);
    }
}

void ReadSocket::handleConnectAccept(Scene &, Socket &socket, rtype_protocol::Packet const &packet,
                                     std::size_t len)
{
    if (len < 2)
        return;

    socket.connected = true;
    socket.playerId = packet[1];
}

void ReadSocket::handleConnectReject(Scene &scene, Socket &, rtype_protocol::Packet const &packet,
                                     std::size_t len)
{
    if (len < 2)
        return;

    std::cerr << "Connection rejected by the server: " << rejectReason(packet[1]) << "\n";
    scene.getResource<AppAlive>().alive = false;
}

void ReadSocket::handlePlayerLeft(Scene &scene, Socket &socket,
                                  rtype_protocol::Packet const &packet, std::size_t len)
{
    if (len < 2)
        return;

    if (socket.connected && packet[1] == socket.playerId) {
        std::cerr << "Player eliminated by the server\n";
        scene.getResource<AppAlive>().alive = false;
    }
}

void ReadSocket::handleGameStarted(Scene &, Socket &socket, rtype_protocol::Packet const &packet,
                                   std::size_t len)
{
    if (len < 5)
        return;
    socket.tick = rtype_protocol::readValue<uint32_t>(&packet[1]);
}

void ReadSocket::handleSnapshot(Scene &, Socket &, rtype_protocol::Packet const &packet,
                                std::size_t len)
{
    if (len < rtype_protocol::SNAPSHOT_HEADER_SIZE)
        return;

    uint32_t tick = rtype_protocol::readValue<uint32_t>(&packet[1]);
    if (tick < _displayedTick)
        return;

    if (_snapshotParts.empty() || tick > _snapshotTick) {
        _snapshotParts.clear();
        _snapshotTick = tick;
    }

    if (tick == _snapshotTick)
        _snapshotParts.emplace_back(packet, len);
}

void ReadSocket::handleDelta(Scene &, Socket &socket, rtype_protocol::Packet const &packet,
                             std::size_t len)
{
    if (len < 5)
        return;

    uint32_t tick = rtype_protocol::readValue<uint32_t>(&packet[1]);
    if (tick > socket.lastSnapshotTick)
        socket.lastSnapshotTick = tick;
}

void ReadSocket::handlePacket(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet,
                              std::size_t len)
{
    if (len == 0)
        return;

    auto dispatch = [&](uint8_t packetType) {
        static const std::unordered_map<uint8_t, void (ReadSocket::*)(
                                                     Scene &, Socket &,
                                                     rtype_protocol::Packet const &, std::size_t)>
            handlers = {{rtype_protocol::CONNECT_ACCEPT, &ReadSocket::handleConnectAccept},
                        {rtype_protocol::CONNECT_REJECT, &ReadSocket::handleConnectReject},
                        {rtype_protocol::PLAYER_LEFT, &ReadSocket::handlePlayerLeft},
                        {rtype_protocol::GAME_STARTED, &ReadSocket::handleGameStarted},
                        {rtype_protocol::SNAPSHOT, &ReadSocket::handleSnapshot},
                        {rtype_protocol::DELTA, &ReadSocket::handleDelta}};

        auto it = handlers.find(packetType);
        if (it != handlers.end())
            (this->*(it->second))(scene, socket, packet, len);
    };

    dispatch(packet[0]);
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
