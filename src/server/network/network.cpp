/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** network
*/

#include "network/network.hpp"
#include "rtype_protocol.hpp"
#include <algorithm>
#include <asio.hpp>
#include <exception>
#include <iostream>
#include <optional>

namespace rtype_server
{
Network::Network(uint16_t port, std::string ip, std::map<uint8_t, InputQueue> &inputs,
                 std::atomic<bool> &running)
    : _endpoint(asio::ip::make_address(ip), port), _socketAsio(_ioContext, _endpoint),
      _playerInputs(inputs), _running(running)
{
    _running = true;
    std::cout << "Server listening on ip : " << ip << std::endl;
    std::cout << "Server listening on port : " << port << std::endl;
}

Network::~Network()
{
    if (_socketAsio.is_open()) {
        std::cout << "Closing server" << std::endl;
        _socketAsio.close();
    }
}

std::map<uint8_t, InputQueue> &Network::getPlayerInputs() { return _playerInputs; }

// Thread reseau : recoit les paquets en continu et range les inputs dans la queue
void Network::receiveLoop()
{
    rtype_protocol::Packet buffer;
    asio::ip::udp::endpoint sender;
    asio::error_code error;

    while (_running) {
        std::size_t n = _socketAsio.receive_from(asio::buffer(buffer), sender, 0, error);
        if (!_running)
            break;
        if (error == asio::error::connection_reset || error == asio::error::connection_refused)
            continue;
        if (error) {
            std::cerr << "Network: " << error.message() << std::endl;
            break;
        }
        if (n == 0)
            continue;
        uint8_t type = static_cast<uint8_t>(buffer[0]);
        std::optional<uint8_t> playerId;
        {
            std::lock_guard<std::mutex> lock(_playersMutex);
            auto player = _players.find(sender);
            if (player != _players.end())
                playerId = player->second;
        }
        if (type != rtype_protocol::CONNECT && !playerId) {
            reply(sender, rtype_protocol::CONNECT_REJECT, rtype_protocol::NOT_CONNECTED);
            continue;
        }
        try {
            switch (type) {
            case rtype_protocol::CONNECT:
                handleConnect(buffer, n, sender);
                break;
            case rtype_protocol::INPUT:
                if (n != rtype_protocol::INPUT_SIZE)
                    break;
                _playerInputs.at(*playerId).push(buffer[5]);
                break;
            case rtype_protocol::DISCONNECT:
                disconnectPlayer(*playerId);
                break;
            }
        } catch (std::exception const &exception) {
            std::cerr << "Dropped packet: " << exception.what() << std::endl;
        }
    }
    _running = false;
}

void Network::stop()
{
    _running = false;
    asio::ip::udp::socket wakeUp(_ioContext);
    asio::error_code error;
    wakeUp.open(asio::ip::udp::v4(), error);
    if (error)
        return;
    asio::ip::address target = _endpoint.address();
    if (target.is_unspecified())
        target = asio::ip::address_v4::loopback();
    wakeUp.send_to(asio::buffer("", 1), asio::ip::udp::endpoint(target, _endpoint.port()), 0, error);
}

void Network::handleConnect(const rtype_protocol::Packet &packet, std::size_t size,
                            const asio::ip::udp::endpoint &sender)
{
    if (size != 2)
        return;
    if (static_cast<uint8_t>(packet[1]) != rtype_protocol::PROTOCOL_VERSION) {
        reply(sender, rtype_protocol::CONNECT_REJECT, rtype_protocol::WRONG_VERSION);
        return;
    }
    std::lock_guard<std::mutex> lock(_playersMutex);
    if (_eliminatedPlayers.find(sender) != _eliminatedPlayers.end()) {
        reply(sender, rtype_protocol::CONNECT_REJECT, rtype_protocol::SERVER_FULL);
        return;
    }
    auto player = _players.find(sender);
    if (player != _players.end()) {
        reply(sender, rtype_protocol::CONNECT_ACCEPT, player->second);
        return;
    }
    for (uint8_t id = 0; id < rtype_protocol::MAX_PLAYERS; id++) {
        bool taken = false;
        for (auto const &[endpoint, playerId] : _players)
            if (playerId == id)
                taken = true;
        if (!taken) {
            _players[sender] = id;
            std::cout << "Player " << static_cast<int>(id)
                      << " connected: " << sender.address().to_string() << ":" << sender.port()
                      << std::endl;
            reply(sender, rtype_protocol::CONNECT_ACCEPT, id);
            return;
        }
    }
    reply(sender, rtype_protocol::CONNECT_REJECT, rtype_protocol::SERVER_FULL);
}

void Network::disconnectPlayer(uint8_t playerId)
{
    std::lock_guard<std::mutex> lock(_playersMutex);
    auto player = std::find_if(_players.begin(), _players.end(),
                               [playerId](auto const &entry) { return entry.second == playerId; });
    if (player == _players.end())
        return;

    const auto endpoint = player->first;
    _eliminatedPlayers.insert(endpoint);

    const uint8_t packet[] = {rtype_protocol::PLAYER_LEFT, playerId};
    for (auto const &[target, unused] : _players) {
        asio::error_code error;
        _socketAsio.send_to(asio::buffer(packet), target, 0, error);
    }
    asio::error_code error;
    _socketAsio.send_to(asio::buffer(packet), endpoint, 0, error);
    _players.erase(player);
}

// [SNAPSHOT, tick (u32), count (u16), entites...], entite = [x (f32), y (f32), type (u8)]
// Garde le snapshot dans l'historique puis le decoupe en plusieurs paquets du meme tick
// si les entites ne tiennent pas dans un Packet
void Network::sendSnapshot(uint32_t tick,
                           const std::vector<rtype_protocol::EntitySnapshot> &entities)
{
    constexpr std::size_t headerSize = rtype_protocol::SNAPSHOT_HEADER_SIZE;
    constexpr std::size_t entitySize = rtype_protocol::ENTITY_SIZE;
    constexpr std::size_t maxPerPacket = (sizeof(rtype_protocol::Packet) - headerSize) / entitySize;
    SnapshotEntry &entry = _snapshotHistory[tick % _snapshotHistory.size()];
    std::vector<asio::ip::udp::endpoint> targets;
    std::size_t sent = 0;

    {
        std::lock_guard<std::mutex> lock(_playersMutex);
        for (auto const &[endpoint, playerId] : _players)
            targets.push_back(endpoint);
    }
    // std::cout << "Sending snapshot tick " << tick << " with " << entities.size() << " entities"
    //           << std::endl;
    entry.tick = tick;
    entry.entities = entities;
    // for (int i = entry.entities.size(); i < 10; i++) {
    //     entry.entities.push_back(rtype_protocol::EntitySnapshot{
    //         Position{10 * static_cast<float>(i), 10 * static_cast<float>(i)},
    //         Type{EntType::PLAYER}});
    // }
    while (sent < entry.entities.size()) {
        rtype_protocol::Packet packet{};
        std::size_t count = std::min(entry.entities.size() - sent, maxPerPacket);
        std::size_t size = headerSize;

        packet[0] = rtype_protocol::SNAPSHOT;
        rtype_protocol::writeValue<uint32_t>(&packet[1], entry.tick);
        packet[5] = static_cast<uint8_t>(count >> 8);
        packet[6] = static_cast<uint8_t>(count);
        for (std::size_t i = sent; i < sent + count; i++) {
            const rtype_protocol::EntitySnapshot &entity = entry.entities[i];

            rtype_protocol::writeValue<float>(&packet[size], entity.pos.x);
            rtype_protocol::writeValue<float>(&packet[size + 4], entity.pos.y);
            packet[size + 8] = static_cast<uint8_t>(entity.type.type);
            size += entitySize;
        }
        for (auto const &endpoint : targets) {
            asio::error_code error;
            _socketAsio.send_to(asio::buffer(packet.data(), size), endpoint, 0, error);
        }
        sent += count;
    }
}

void Network::reply(const asio::ip::udp::endpoint &target, uint8_t type, uint8_t value)
{
    const uint8_t packet[] = {type, value};
    asio::error_code error;

    _socketAsio.send_to(asio::buffer(packet), target, 0, error);
}

std::vector<uint8_t> Network::getConnectedPlayerIds()
{
    std::lock_guard<std::mutex> lock(_playersMutex);
    std::vector<uint8_t> ids;
    for (auto const &[endpoint, id] : _players)
        ids.push_back(id);
    return ids;
}
} // namespace rtype_server
