/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** NetworkPlugin
*/

#include "NetworkPlugin.hpp"
#include "Events.hpp"
#include "NetEvents.hpp"
#include "NetReceiveSystem.hpp"
#include "NetSendSystem.hpp"
#include "NetTransport.hpp"
#include "PluginApi.hpp"
#include "UdpSocket.hpp"
#include <optional>

void NetworkPlugin::build(Application &app) { build(app._scene); }

void NetworkPlugin::cleanup(Application &app) { cleanup(app._scene); }

void NetworkPlugin::build(Scene &scene)
{
    auto const &config = scene.getResource<NetworkConfig>();
    std::optional<Endpoint> server;
    if (config.role == NetRole::Client) {
        Endpoint endpoint{};
        scene.addResource<UdpSocket>(UdpSocket::bindClient(config.host, config.port, endpoint));
        server = endpoint;
    } else {
        scene.addResource<UdpSocket>(UdpSocket::bindServer(config.port, config.host));
    }
    scene.addResource<NetTransport>(config, server);
    scene.addResource<Events<PeerConnected>>();
    scene.addResource<Events<PeerDisconnected>>();
    scene.addResource<Events<NetMessage>>();

    scene.registerSystem<EventUpdateSystem<PeerConnected>>(PrepUpdate);
    scene.registerSystem<EventUpdateSystem<PeerDisconnected>>(PrepUpdate);
    scene.registerSystem<EventUpdateSystem<NetMessage>>(PrepUpdate);
    scene.registerSystem<NetReceiveSystem>(PrepUpdate);
    scene.registerSystem<NetSendSystem>(PostUpdate);
}

void NetworkPlugin::cleanup(Scene &scene)
{
    if (!scene.hasResource<NetTransport>() || !scene.hasResource<UdpSocket>())
        return;
    auto &transport = scene.getResource<NetTransport>();

    // A client's only peer is the server, id 0; disconnect(0) also abandons a handshake
    if (scene.getResource<NetworkConfig>().role == NetRole::Client) {
        if (transport.state() != ConnectionState::Disconnected)
            transport.disconnect(0);
    } else {
        for (PeerId peer : transport.peers())
            transport.disconnect(peer);
    }
    flushToSocket(transport, scene.getResource<UdpSocket>());
}

PLUGIN_EXPORT IPlugin *createPlugin() { return new NetworkPlugin(); }
