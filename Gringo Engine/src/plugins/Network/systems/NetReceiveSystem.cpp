/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetReceiveSystem
*/

#include "NetReceiveSystem.hpp"
#include "Events.hpp"
#include "NetTransport.hpp"
#include "NetworkConfig.hpp"
#include "UdpSocket.hpp"
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
    for (auto const &event : incoming.connected)
        connected.send(event);
    for (auto const &event : incoming.disconnected)
        disconnected.send(event);
    for (auto &event : incoming.messages)
        messages.send(std::move(event));
}
