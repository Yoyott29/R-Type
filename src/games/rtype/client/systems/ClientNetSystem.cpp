/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ClientNetSystem
*/

#include "ClientNetSystem.hpp"
#include "AppAlive.hpp"
#include "ByteStream.hpp"
#include "ClientSession.hpp"
#include "Position.hpp"
#include "RTypeMessages.hpp"
#include "Sprite.hpp"
#include "Type.hpp"
#include "rtype_protocol.hpp"
#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace
{
std::array<std::string, rtype_protocol::MAX_PLAYERS> const PLAYER_TEXTURES = {{
    "Assets/PlayerGreen.png",
    "Assets/PlayerBlue.png",
    "Assets/PlayerMagenta.png",
    "Assets/PlayerRed.png",
}};

std::string texturePathForType(EntType type, std::uint8_t variant)
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

void createEntity(Scene &scene, SnapshotEntity const &snapshot)
{
    ColorRGBA const color{snapshot.color[0], snapshot.color[1], snapshot.color[2],
                          snapshot.color[3]};
    Sprite sprite =
        snapshot.shape == rtype_protocol::CIRCLE_SHAPE
            ? Sprite{CircleShape{snapshot.width}, color, snapshot.drawOrder}
            : Sprite{BoxShape{snapshot.width, snapshot.height}, color, snapshot.drawOrder};
    sprite.texturePath = texturePathForType(snapshot.type, snapshot.variant);
    if (snapshot.type == EntType::PLAYER)
        sprite.texturePath = PLAYER_TEXTURES.at(snapshot.playerId); // decodeSnapshot checked it
    Entity entity = scene.createEntity();
    scene.addComponent(entity, Position{snapshot.x, snapshot.y});
    scene.addComponent(entity, Type{snapshot.type});
    scene.addComponent(entity, sprite);
}

// Shows the newest tick of this frame; older ticks than the displayed one are dropped
void applySnapshots(Scene &scene, ClientSession &session,
                    std::vector<SnapshotMessage> const &chunks)
{
    if (chunks.empty())
        return;
    std::uint32_t newest = chunks.front().tick;
    for (SnapshotMessage const &chunk : chunks)
        newest = std::max(newest, chunk.tick);
    if (session.displayedTick && newest < *session.displayedTick)
        return;
    if (!session.displayedTick || newest > *session.displayedTick) {
        scene.putAllEntitysInDestroyQueue();
        scene.flushDestroyQueue();
        session.displayedTick = newest;
    }
    for (SnapshotMessage const &chunk : chunks)
        if (chunk.tick == newest)
            for (SnapshotEntity const &entity : chunk.entities)
                createEntity(scene, entity);
}

char const *disconnectText(DisconnectReason reason)
{
    switch (reason) {
    case DisconnectReason::Remote:
        return "Disconnected by the server";
    case DisconnectReason::Timeout:
        return "Timeout: lost connection to the server";
    case DisconnectReason::ConnectFailed:
        return "Could not connect to the server";
    case DisconnectReason::Full:
        return "Connection rejected by the server: server full";
    case DisconnectReason::VersionMismatch:
        return "Connection rejected by the server: wrong protocol version";
    }
    return "Disconnected";
}
} // namespace

void ClientNetSystem::update(Scene &scene)
{
    auto &session = scene.getResource<ClientSession>();
    std::vector<SnapshotMessage> snapshots;

    _messages.read(scene, [&](NetMessage const &message) {
        try {
            if (message.type == static_cast<std::uint16_t>(RTypeMessage::Welcome)) {
                if (session.playerId)
                    return; // repeated Welcome
                session.playerId = decodeWelcome(message.payload).playerId;
                std::cout << "Connected to " << session.server << " as player "
                          << static_cast<int>(*session.playerId) << '\n'
                          << std::flush;
            } else if (message.type == static_cast<std::uint16_t>(RTypeMessage::Snapshot)) {
                snapshots.push_back(decodeSnapshot(message.payload));
            }
        } catch (NetError const &) {
            return; // Malformed message: ignored
        }
    });
    applySnapshots(scene, session, snapshots);

    _disconnected.read(scene, [&](PeerDisconnected const &event) {
        std::cerr << disconnectText(event.reason) << '\n' << std::flush;
        scene.getResource<AppAlive>().alive = false;
    });
}
