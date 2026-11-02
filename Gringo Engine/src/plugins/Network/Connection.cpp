/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Connection
*/

#include "Connection.hpp"
#include <utility>

bool seqNewer(std::uint16_t a, std::uint16_t b)
{
    return a != b && static_cast<std::uint16_t>(a - b) < 0x8000;
}

Connection::Connection(NetworkConfig const &config, TimePoint now)
    : _config(config), _lastSend(now), _lastReceive(now)
{
}

void Connection::queue(std::uint16_t type, std::span<const std::byte> payload)
{
    _queued.push_back({type, std::vector<std::byte>(payload.begin(), payload.end())});
}

std::vector<WireMessage> Connection::takeQueued() { return std::exchange(_queued, {}); }

void Connection::stamp(Packet &packet, TimePoint now)
{
    packet.seq = _localSeq++;
    _lastSend = now;
}

bool Connection::keepAliveDue(TimePoint now) const
{
    return now - _lastSend >= _config.keepAliveInterval;
}

bool Connection::receive(Packet const &packet, TimePoint now)
{
    if (!_hasRemote) {
        _hasRemote = true;
        _remoteSeq = packet.seq;
        _remoteBits = 0;
    } else if (seqNewer(packet.seq, _remoteSeq)) {
        std::uint16_t const shift = packet.seq - _remoteSeq;
        _stats.missing += shift - 1u;
        std::uint64_t const bits = (static_cast<std::uint64_t>(_remoteBits) << 1) | 1u;
        _remoteBits = shift > RECEIVE_WINDOW ? 0 : static_cast<std::uint32_t>(bits << (shift - 1u));
        _remoteSeq = packet.seq;
    } else {
        unsigned const behind = static_cast<std::uint16_t>(_remoteSeq - packet.seq);
        if (behind == 0 || behind > RECEIVE_WINDOW) {
            if (behind == 0)
                _stats.duplicate++;
            else
                _stats.late++;
            return false;
        }
        std::uint32_t const bit = 1u << (behind - 1);
        if ((_remoteBits & bit) != 0) {
            _stats.duplicate++;
            return false;
        }
        _remoteBits |= bit;
        _stats.late++;
        if (_stats.missing > 0)
            _stats.missing--;
    }
    _stats.received++;
    _lastReceive = now;
    return true;
}

bool Connection::timedOut(TimePoint now) const { return now - _lastReceive >= _config.timeout; }

ConnectionStats Connection::takeStats() { return std::exchange(_stats, {}); }
