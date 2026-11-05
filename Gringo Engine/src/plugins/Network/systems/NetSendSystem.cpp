/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetSendSystem
*/

#include "NetSendSystem.hpp"
#include "NetTransport.hpp"
#include "NetworkConfig.hpp"
#include "UdpSocket.hpp"

void flushToSocket(NetTransport &transport, UdpSocket &socket)
{
    transport.flush(
        NetClock::now(),
        [&socket](Endpoint const &to, std::span<const std::byte> data) { socket.send(to, data); });
}

void NetSendSystem::update(Scene &scene)
{
    flushToSocket(scene.getResource<NetTransport>(), scene.getResource<UdpSocket>());
}
