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
std::array<std::string, rtype_protocol::MAX_PLAYERS> const playerTextures = {{
    "Assets/PlayerGreen.png",
    "Assets/PlayerBlue.png",
    "Assets/PlayerMagenta.png",
    "Assets/PlayerRed.png",
}};

std::string texturePathForType(EntType type, uint8_t variant)
{
    switch (type) {
    case EntType::ENEMY:
        return variant == rtype_protocol::ENEMY_VARIANT_2 ? "Assets/Bydos2.png"
                                                          : "Assets/Bydos1.png";
    case EntType::BULLET:
        return "Assets/Bullet.png";
    default:
        return {};
    }
}
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
    if (socket.connected &&
        std::chrono::steady_clock::now() - _lastSnapshotReceived > SNAPSHOT_TIMEOUT) {
        std::cerr << "Timeout: No snapshot received for " << SNAPSHOT_TIMEOUT.count()
                  << " seconds, disconnecting\n";
        scene.getResource<AppAlive>().alive = false;
    }
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
        uint8_t playerId = packet[i + 9];
        entity.pos.x = rtype_protocol::readValue<float>(&packet[i]);
        entity.pos.y = rtype_protocol::readValue<float>(&packet[i + 4]);
        if (type > EntType::OTHER) {
            throw UnknownComponentType(std::to_string(type));
        }
        entity.type.type = static_cast<EntType>(type);
        entity.playerId = playerId;
        entity.variant = packet[i + 10];
        entity.width = rtype_protocol::readValue<float>(&packet[i + 11]);
        entity.height = rtype_protocol::readValue<float>(&packet[i + 15]);
        entity.shape = packet[i + 19];
        entity.drawOrder = static_cast<int8_t>(packet[i + 20]);
        for (std::size_t color = 0; color < 4; color++)
            entity.color[color] = packet[i + 21 + color];

        ColorRGBA color{entity.color[0], entity.color[1], entity.color[2], entity.color[3]};
        Sprite sprite =
            entity.shape == rtype_protocol::CIRCLE_SHAPE
                ? Sprite{CircleShape{entity.width}, color, entity.drawOrder}
                : Sprite{BoxShape{entity.width, entity.height}, color, entity.drawOrder};
        sprite.texturePath = texturePathForType(entity.type.type, entity.variant);
        if (entity.type.type == EntType::PLAYER) {
            if (playerId >= rtype_protocol::MAX_PLAYERS)
                throw UnknownComponentType("invalid player id: " + std::to_string(playerId));
            sprite.texturePath = playerTextures[playerId];
        }
        Entity e = scene.createEntity();
        scene.addComponent(e, entity.pos);
        scene.addComponent(e, entity.type);
        scene.addComponent(e, sprite);
    }
}

void ReadSocket::handleConnectAccept(Scene &, Socket &socket, rtype_protocol::Packet const &packet,
                                     std::size_t len)
{
    if (len < 2)
        return;

    socket.connected = true;
    socket.playerId = packet[1];
    _lastSnapshotReceived = std::chrono::steady_clock::now();
    std::cout << "Connected to " << socket.serverAddress << ":" << socket.serverPort
              << " as player " << static_cast<int>(socket.playerId) << "\n";
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

    _lastSnapshotReceived = std::chrono::steady_clock::now();

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
