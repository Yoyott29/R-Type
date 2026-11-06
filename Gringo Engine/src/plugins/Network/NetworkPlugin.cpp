/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetworkPlugin
*/

#include "NetworkPlugin.hpp"
#include "BuildVariantName.hpp"
#include "CommandLine.hpp"
#include "ComponentJson.hpp"
#include "EngineError.hpp"
#include "Events.hpp"
#include "InputSendSystem.hpp"
#include "LocalPeer.hpp"
#include "MirrorSystem.hpp"
#include "NetComponents.hpp"
#include "NetEvents.hpp"
#include "NetReceiveSystem.hpp"
#include "NetSendSystem.hpp"
#include "NetTransport.hpp"
#include "Networked.hpp"
#include "PlayerSystem.hpp"
#include "ProjectSettings.hpp"
#include "RemoteInputSystem.hpp"
#include "Replication.hpp"
#include "ReplicationSystem.hpp"
#include "ReservedInbox.hpp"
#include "SideFilterSystem.hpp"
#include "UdpSocket.hpp"
#include <asio.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

const std::string SERVER_VARIANT = "server";
const std::string CLIENT_VARIANT = "client";
const char PREFAB_SEPARATOR = ',';
const std::string PREFAB_BLANKS = " \t";

// The comma-separated names, each trimmed, without the empty ones
static std::vector<std::string> parsePrefabList(std::string const &text)
{
    std::vector<std::string> names;
    std::size_t start = 0;
    while (start <= text.size()) {
        std::size_t end = text.find(PREFAB_SEPARATOR, start);
        if (end == std::string::npos)
            end = text.size();
        std::string const item = text.substr(start, end - start);
        std::size_t const first = item.find_first_not_of(PREFAB_BLANKS);
        if (first != std::string::npos)
            names.push_back(item.substr(first, item.find_last_not_of(PREFAB_BLANKS) - first + 1));
        start = end + 1;
    }
    return names;
}

void NetworkPlugin::registerComponents(Scene &scene)
{
    scene.registerComponent<Networked>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            Networked networked;
            networked.mode = Networked::parseMode(ComponentJson::value(data, "Mode", "Replicated"));
            networked.prefab = ComponentJson::value(data, "Prefab", "");
            target.addComponent(entity, networked);
        });
    scene.registerComponent<Mirror>();
    scene.registerComponent<NetId>();
    scene.registerComponent<RemoteInput>();
}

void NetworkPlugin::build(Application &app) { build(app._scene); }

void NetworkPlugin::cleanup(Application &app) { cleanup(app._scene); }

std::vector<PropertyDescriptor> NetworkPlugin::settings() const
{
    return {
        {"Host", valueType::String, "127.0.0.1"},
        {"Port", valueType::Int, "4242"},
        {"PlayerPrefabs", valueType::String, ""},
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

    NetworkConfig config = variant == SERVER_VARIANT ? NetworkConfig::server(port, host)
                                                     : NetworkConfig::client(host, port);
    config.playerPrefabs = parsePrefabList(settings.get("Network", "PlayerPrefabs"));
    return config;
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
    scene.addResource<Events<PeerLeft>>();
    scene.addResource<ReservedInbox>();
    replicate<Transform>(scene, "Transform");
    if (config.role == NetRole::Client)
        scene.addResource<LocalPeer>();

    scene.registerSystem<EventUpdateSystem<PeerConnected>>(PrepUpdate);
    scene.registerSystem<EventUpdateSystem<PeerDisconnected>>(PrepUpdate);
    scene.registerSystem<EventUpdateSystem<NetMessage>>(PrepUpdate);
    scene.registerSystem<EventUpdateSystem<PeerLeft>>(PrepUpdate);
    scene.registerSystem<NetReceiveSystem>(PrepUpdate);
    scene.registerSystem<SideFilterSystem>(PrepUpdate);
    if (config.role == NetRole::Client) {
        scene.registerSystem<MirrorSystem>(PrepUpdate);
        scene.registerSystem<InputSendSystem>(PostUpdate);
    } else {
        scene.registerSystem<PlayerSystem>(PrepUpdate);
        scene.registerSystem<RemoteInputSystem>(PrepUpdate);
        scene.registerSystem<ReplicationSystem>(PostUpdate);
    }
    scene.registerSystem<NetSendSystem>(PostUpdate);
}

void NetworkPlugin::cleanup(Scene &scene)
{
    if (!scene.hasResource<NetTransport>() || !scene.hasResource<UdpSocket>())
        return;
    auto &transport = scene.getResource<NetTransport>();

    // A client's only peer is the server; disconnect(SERVER_PEER) also abandons a handshake
    if (scene.getResource<NetworkConfig>().role == NetRole::Client) {
        if (transport.state() != ConnectionState::Disconnected)
            transport.disconnect(SERVER_PEER);
    } else {
        for (PeerId peer : transport.peers())
            transport.disconnect(peer);
    }
    auto &socket = scene.getResource<UdpSocket>();
    flushToSocket(transport, socket);
    // Sends the Disconnect packets just handed over, then stops the I/O thread
    socket.close();
}
