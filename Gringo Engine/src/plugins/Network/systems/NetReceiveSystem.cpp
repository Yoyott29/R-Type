/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetReceiveSystem
*/

#include "NetReceiveSystem.hpp"
#include "Events.hpp"
#include "LocalPeer.hpp"
#include "NetTransport.hpp"
#include "NetworkConfig.hpp"
#include "ReservedInbox.hpp"
#include "UdpSocket.hpp"
#include <iostream>
#include <string>
#include <utility>

void NetReceiveSystem::update(Scene &scene)
{
    auto &socket = scene.getResource<UdpSocket>();
    auto &transport = scene.getResource<NetTransport>();
    TimePoint const now = NetClock::now();

    for (Datagram const &datagram : socket.drain())
        transport.receive(datagram.from, datagram.bytes, now);
    transport.update(now);

    NetIncoming incoming = transport.takeEvents();
    auto &connected = scene.getResource<Events<PeerConnected>>();
    auto &disconnected = scene.getResource<Events<PeerDisconnected>>();
    auto &messages = scene.getResource<Events<NetMessage>>();
    auto &inbox = scene.getResource<ReservedInbox>();
    NetRole const role = scene.getResource<NetworkConfig>().role;
    bool const server = role == NetRole::Server;
    for (auto const &event : incoming.connected) {
        connected.send(event);
        if (server)
            std::cout << "Player " << event.peer << " connected" << std::endl;
        else
            scene.getResource<LocalPeer>().id = transport.localPeer().value_or(NO_PEER);
    }
    for (auto const &event : incoming.disconnected) {
        disconnected.send(event);
        if (server)
            std::cout << "Player " << event.peer
                      << (event.reason == DisconnectReason::Timeout ? " timed out" : " left")
                      << std::endl;
        else
            scene.getResource<LocalPeer>().id = NO_PEER;
    }
    inbox.messages.clear();
    for (auto &event : incoming.messages) {
        if (event.type < RESERVED_TYPE_MIN) {
            messages.send(std::move(event));
            continue;
        }
        if (acceptsReserved(role, static_cast<ReservedType>(event.type)))
            inbox.messages.push_back(std::move(event));
        else
            _log("reserved" + std::to_string(event.type),
                 "dropped reserved message type " + std::to_string(event.type));
    }
}
