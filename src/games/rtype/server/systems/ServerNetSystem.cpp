/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ServerNetSystem
*/

#include "ServerNetSystem.hpp"
#include "ByteStream.hpp"
#include "Collider2D.hpp"
#include "NetTransport.hpp"
#include "PlayerId.hpp"
#include "PlayerScript.hpp"
#include "Position.hpp"
#include "RTypeMessages.hpp"
#include "ServerPlayers.hpp"
#include "Sprite.hpp"
#include "Type.hpp"
#include "Velocity.hpp"
#include <array>
#include <iostream>
#include <memory>

namespace
{
constexpr std::size_t MAX_BUFFERED_INPUTS = 2;

std::array<ColorRGBA, rtype_protocol::MAX_PLAYERS> const PLAYER_COLORS{
    ColorRGBA{0, 228, 48, 255}, ColorRGBA{0, 0, 255, 255}, ColorRGBA{255, 0, 255, 255},
    ColorRGBA{255, 0, 0, 255}};

Entity spawnShip(Scene &scene, std::uint8_t slot)
{
    Entity ship = scene.createEntity();
    scene.addComponent(ship, Type{EntType::PLAYER});
    scene.addComponent(ship, PlayerId{slot});
    scene.addComponent(ship, Position{100.0f, 200.0f + 100.0f * static_cast<float>(slot)});
    scene.addComponent(ship, Velocity{0.0f, 0.0f});
    scene.addComponent(ship, Sprite{BoxShape{103.0f, 50.0f}, PLAYER_COLORS.at(slot)});
    scene.addComponent(ship,
                       Collider2D{BoxShape{103.0f, 50.0f}, [&scene](Entity a, Entity other) {
                                      if (!scene.isEntityAlive(other) ||
                                          scene.getComponent<Type>(other).type != EntType::ENEMY)
                                          return;
                                      auto peer = scene.getResource<ServerPlayers>().peerOf(a);
                                      if (peer)
                                          eliminatePlayer(scene, *peer);
                                  }});
    scene.addComponent(ship, Script{std::make_shared<PlayerScript>()});
    return ship;
}
} // namespace

void ServerNetSystem::update(Scene &scene)
{
    auto &transport = scene.getResource<NetTransport>();
    auto &players = scene.getResource<ServerPlayers>();

    _connected.read(scene, [&](PeerConnected const &event) {
        auto slot = players.freeSlot();
        if (!slot) {
            transport.disconnect(event.peer);
            return;
        }
        players.byPeer.emplace(event.peer, ServerPlayer{*slot, spawnShip(scene, *slot), {}, 0.0f});
        transport.send(event.peer, static_cast<std::uint16_t>(RTypeMessage::Welcome),
                       encodeWelcome({*slot}));
        std::cout << "Player " << static_cast<int>(*slot) << " connected" << '\n' << std::flush;
    });

    _disconnected.read(scene, [&](PeerDisconnected const &event) {
        auto it = players.byPeer.find(event.peer);
        if (it == players.byPeer.end())
            return;
        std::cout << "Player " << static_cast<int>(it->second.slot)
                  << (event.reason == DisconnectReason::Timeout ? " timed out" : " left") << '\n'
                  << std::flush;
        scene.queueDestroy(it->second.entity);
        players.byPeer.erase(it);
    });

    _messages.read(scene, [&](NetMessage const &message) {
        if (message.type != static_cast<std::uint16_t>(RTypeMessage::Input))
            return;
        auto it = players.byPeer.find(message.peer);
        if (it == players.byPeer.end())
            return;
        try {
            InputMessage const input = decodeInput(message.payload);
            // Messages can be lost: repeat the Welcome while the client keeps sending inputs
            transport.send(message.peer, static_cast<std::uint16_t>(RTypeMessage::Welcome),
                           encodeWelcome({it->second.slot}));
            auto &queue = it->second.inputs;
            queue.push_back(input.buttons);
            while (queue.size() > MAX_BUFFERED_INPUTS)
                queue.pop_front();
        } catch (NetError const &) {
            return; // Malformed input: ignored
        }
    });
}
