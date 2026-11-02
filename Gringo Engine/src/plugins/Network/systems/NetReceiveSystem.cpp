/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** NetReceiveSystem
*/

#include "NetReceiveSystem.hpp"
#include "Events.hpp"
#include "NetTransport.hpp"
#include "NetworkConfig.hpp"
#include "UdpSocket.hpp"
#include <array>
#include <cstddef>
#include <utility>

static constexpr std::size_t ReadBufferSize = 1500;

void NetReceiveSystem::update(Scene &scene)
{
    auto &socket = scene.getResource<UdpSocket>();
    auto &transport = scene.getResource<NetTransport>();
    TimePoint const now = NetClock::now();

    std::array<std::byte, ReadBufferSize> buffer;
    Endpoint from{};
    while (auto const size = socket.receive(buffer, from))
        transport.receive(from, std::span<const std::byte>(buffer).first(*size), now);
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
