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
#include <cstdint>
#include <map>
#include <mutex>
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
    Network(uint16_t port, std::map<uint8_t, InputQueue> &inputs, std::atomic<bool> &running);
    ~Network();
    void receiveLoop();
    void sendSnapshot(uint32_t tick, const std::vector<rtype_protocol::EntitySnapshot> &entities);

    const std::map<asio::ip::udp::endpoint, uint8_t> &getPlayers() const; // endpoint -> playerId
    std::map<uint8_t, InputQueue> &getPlayerInputs();                     // playerId -> InputQueue

  private:
    void handleConnect(const rtype_protocol::Packet &packet, std::size_t size,
                       const asio::ip::udp::endpoint &sender);
    void reply(const asio::ip::udp::endpoint &target, uint8_t type, uint8_t value);

    asio::io_context _ioContext;
    asio::ip::udp::endpoint _endpoint;
    asio::ip::udp::socket _socketAsio;
    std::map<uint8_t, InputQueue> &_playerInputs; // playerId -> InputQueue
    std::atomic<bool> &_running;
    std::map<asio::ip::udp::endpoint, uint8_t> _players; // endpoint -> playerId
    std::mutex _playersMutex;
    std::array<SnapshotEntry, 64> _snapshotHistory; // tick % 64 -> snapshot envoye
};
} // namespace rtype_server
