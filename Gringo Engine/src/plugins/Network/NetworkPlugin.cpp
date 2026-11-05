/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetworkPlugin
*/

#include "NetworkPlugin.hpp"
#include "BuildVariantName.hpp"
#include "CommandLine.hpp"
#include "EngineError.hpp"
#include "Events.hpp"
#include "NetEvents.hpp"
#include "NetReceiveSystem.hpp"
#include "NetSendSystem.hpp"
#include "NetTransport.hpp"
#include "ProjectSettings.hpp"
#include "UdpSocket.hpp"
#include <asio.hpp>
#include <optional>

static constexpr const char *SERVER_VARIANT = "server";
static constexpr const char *CLIENT_VARIANT = "client";

void NetworkPlugin::build(Application &app) { build(app._scene); }

void NetworkPlugin::cleanup(Application &app) { cleanup(app._scene); }

std::vector<PropertyDescriptor> NetworkPlugin::settings() const
{
    return {
        {"Host", valueType::String, "127.0.0.1"},
        {"Port", valueType::Int, "4242"},
    };
}

std::vector<BuildVariant> NetworkPlugin::buildVariants() const
{
    return {{SERVER_VARIANT, true}, {CLIENT_VARIANT, false}};
}

std::uint16_t NetworkPlugin::parsePort(const std::string &text)
{
    // Digits only, so std::stoul cannot throw nor accept "-1" or " 42"
    if (text.empty() || text.size() > 5 ||
        text.find_first_not_of("0123456789") != std::string::npos)
        throw EngineError("Invalid port: " + text);
    const unsigned long port = std::stoul(text);
    if (port == 0 || port > 65535)
        throw EngineError("Invalid port: " + text);
    return static_cast<std::uint16_t>(port);
}

NetworkConfig NetworkPlugin::resolveConfig(Scene &scene)
{
    if (scene.hasResource<NetworkConfig>())
        return scene.getResource<NetworkConfig>();
    const std::string variant =
        scene.hasResource<BuildVariantName>() ? scene.getResource<BuildVariantName>().name : "";
    if (variant.empty())
        throw EngineError("Network: no build variant and no NetworkConfig resource");
    if (variant != SERVER_VARIANT && variant != CLIENT_VARIANT)
        throw EngineError("Network: unknown build variant " + variant);
    const ProjectSettings &settings = scene.getResource<ProjectSettings>();
    const std::vector<std::string> &arguments = scene.getResource<CommandLine>().arguments;
    std::string host = settings.get("Network", "Host");
    std::uint16_t port = parsePort(settings.get("Network", "Port"));

    // The command line wins over the project: "port" or "host port"
    if (arguments.size() > 2)
        throw EngineError("Usage: [ip] port");
    if (!arguments.empty())
        port = parsePort(arguments.back());
    if (arguments.size() == 2)
        host = arguments.front();
    asio::error_code error;
    asio::ip::make_address_v4(host, error);
    if (error)
        throw EngineError("Invalid ip: " + host);

    if (variant == SERVER_VARIANT)
        return NetworkConfig::server(port, host);
    return NetworkConfig::client(host, port);
}

void NetworkPlugin::build(Scene &scene)
{
    if (!scene.hasResource<NetworkConfig>())
        scene.addResource<NetworkConfig>(resolveConfig(scene));
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
