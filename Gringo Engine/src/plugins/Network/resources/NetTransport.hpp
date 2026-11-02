/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** NetTransport
*/

#pragma once
#include "Connection.hpp"
#include "NetEvents.hpp"
#include "NetworkConfig.hpp"
#include "Packet.hpp"
#include "UdpSocket.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

enum class ConnectionState { Disconnected, Connecting, Connected };

struct NetIncoming {
    std::vector<PeerConnected> connected;
    std::vector<PeerDisconnected> disconnected;
    std::vector<NetMessage> messages;
};

using SendFn = std::function<void(Endpoint const &, std::span<const std::byte>)>;

// Socket-free peer table, handshake and lifecycle; never reads the clock, never throws
class NetTransport
{
  public:
    // `server` is required for a Client (which starts Connecting) and ignored for a Server
    NetTransport(NetworkConfig config, std::optional<Endpoint> server);

    void send(PeerId peer, std::uint16_t type, std::span<const std::byte> payload);
    void broadcast(std::uint16_t type, std::span<const std::byte> payload);
    void disconnect(PeerId peer);
    std::vector<PeerId> peers() const;
    ConnectionState state() const;

    void receive(Endpoint const &from, std::span<const std::byte> datagram, TimePoint now);
    // Timeouts and ConnectFailed
    void update(TimePoint now);
    // ConnectRequest retries, queued control packets, then per peer data else a due KeepAlive
    void flush(TimePoint now, SendFn const &sendTo);
    NetIncoming takeEvents();

  private:
    struct Peer {
        PeerId id;
        Connection connection;
        bool connected = false;     // client: false until ConnectAccepted
        bool acceptPending = false; // server: a ConnectAccepted is owed
    };

    Peer *findPeer(PeerId id);
    void removePeer(PeerId id);
    void sendPacket(Endpoint const &to, Packet packet, Peer &peer, TimePoint now,
                    SendFn const &sendTo);
    void receiveFromUnknown(Endpoint const &from, Packet const &packet, TimePoint now);
    void receiveFromPeer(Peer &peer, Packet &packet, TimePoint now);
    void receiveWhileConnecting(Peer &peer, Packet const &packet, TimePoint now);
    void flushConnectRequest(TimePoint now, SendFn const &sendTo);
    void flushPeer(Endpoint const &to, Peer &peer, TimePoint now, SendFn const &sendTo);
    // Emits the event, drops the peer and, on a client, ends the session for good
    void endPeer(PeerId id, DisconnectReason reason);
    // Once per second when config.logStats: one stderr line per connected peer
    void logStats(TimePoint now);

    NetworkConfig _config;
    std::optional<Endpoint> _server; // client only
    ConnectionState _state;
    TimePoint _now{}; // latest time seen, for disconnect() which takes none

    std::unordered_map<Endpoint, Peer> _peers;
    std::unordered_map<PeerId, Endpoint> _endpoints;
    PeerId _nextId = 1;

    // Client handshake
    int _attempts = 0;
    std::optional<TimePoint> _lastRequest;

    // Encoded packets for endpoints without (or no longer with) a Connection
    std::vector<std::pair<Endpoint, std::vector<std::byte>>> _outbox;
    NetIncoming _events;
    bool _oversizedLogged = false;
    std::optional<TimePoint> _lastStatsLog;
};
