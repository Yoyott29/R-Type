/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** network
*/

#pragma once
#include "input/input_queue.hpp"
#include "rtype_protocol.hpp"
#include <array>
#include <asio.hpp>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <map>
#include <mutex>
#include <set>
#include <vector>

namespace rtype_server
{

struct SnapshotEntry {
    uint32_t tick;
    std::vector<rtype_protocol::EntitySnapshot> entities;
};

class Network
{
  public:
    Network(uint16_t port, std::string ip, std::map<uint8_t, InputQueue> &inputs,
            std::atomic<bool> &running);
    ~Network();
    void disconnectPlayer(uint8_t playerId);
    void receiveLoop();
    void stop();
    void sendSnapshot(uint32_t tick, const std::vector<rtype_protocol::EntitySnapshot> &entities);

    std::map<uint8_t, InputQueue> &getPlayerInputs(); // playerId -> InputQueue
    std::vector<uint8_t> getConnectedPlayerIds();
    void dropTimedOutPlayers();

  private:
    static constexpr std::chrono::seconds PLAYER_TIMEOUT{2};

    void handleConnect(const rtype_protocol::Packet &packet, std::size_t size,
                       const asio::ip::udp::endpoint &sender);
    void reply(const asio::ip::udp::endpoint &target, uint8_t type, uint8_t value);

    asio::io_context _ioContext;
    asio::ip::udp::endpoint _endpoint;
    asio::ip::udp::socket _socketAsio;
    std::map<uint8_t, InputQueue> &_playerInputs; // playerId -> InputQueue
    std::atomic<bool> &_running;
    std::map<asio::ip::udp::endpoint, uint8_t> _players; // endpoint -> playerId
    std::set<asio::ip::udp::endpoint> _eliminatedPlayers;
    std::map<uint8_t, std::chrono::steady_clock::time_point> _lastSeen;
    std::mutex _playersMutex;
    std::array<SnapshotEntry, 64> _snapshotHistory; // tick % 64 -> snapshot envoye
};
} // namespace rtype_server
