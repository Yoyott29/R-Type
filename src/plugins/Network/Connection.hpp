/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Connection
*/

#pragma once
#include "NetworkConfig.hpp"
#include "Packet.hpp"
#include <cstdint>
#include <span>
#include <vector>

bool seqNewer(std::uint16_t a, std::uint16_t b);

// Incoming packet counters since the last takeStats()
struct ConnectionStats {
    std::uint32_t received = 0;
    std::uint32_t missing = 0;   // seq gaps not (yet) filled
    std::uint32_t late = 0;      // arrived out of order
    std::uint32_t duplicate = 0;
};

class Connection
{
  public:
    Connection(NetworkConfig const &config, TimePoint now);

    void queue(std::uint16_t type, std::span<const std::byte> payload);
    std::vector<WireMessage> takeQueued();
    void stamp(Packet &packet, TimePoint now);
    bool keepAliveDue(TimePoint now) const;
    bool receive(Packet const &packet, TimePoint now);
    bool timedOut(TimePoint now) const;
    ConnectionStats takeStats();

  private:
    static constexpr std::uint16_t RECEIVE_WINDOW = 32;

    NetworkConfig _config;
    TimePoint _lastSend;
    TimePoint _lastReceive;

    std::uint16_t _localSeq = 0;
    std::vector<WireMessage> _queued;

    bool _hasRemote = false;
    std::uint16_t _remoteSeq = 0;
    std::uint32_t _remoteBits = 0;
    ConnectionStats _stats;
};
