/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** NetTransport
*/

#include "NetTransport.hpp"
#include <algorithm>
#include <iostream>

static Packet makePacket(PacketKind kind)
{
    Packet packet;
    packet.kind = kind;
    return packet;
}

NetTransport::NetTransport(NetworkConfig config, std::optional<Endpoint> server)
    : _config(std::move(config)), _server(_config.role == NetRole::Client ? server : std::nullopt),
      _state(_config.role == NetRole::Server ? ConnectionState::Connected
             : _server                       ? ConnectionState::Connecting
                                             : ConnectionState::Disconnected)
{
}

void NetTransport::send(PeerId peer, std::uint16_t type, std::span<const std::byte> payload)
{
    Peer *target = findPeer(peer);
    if (target == nullptr || !target->connected)
        return;
    if (payload.size() > MAX_PAYLOAD) {
        if (!_oversizedLogged) {
            _oversizedLogged = true;
            std::cerr << "NetworkPlugin: dropped a " << payload.size()
                      << "-byte send, over MAX_PAYLOAD " << MAX_PAYLOAD << "\n";
        }
        return;
    }
    target->connection.queue(type, payload);
}

void NetTransport::broadcast(std::uint16_t type, std::span<const std::byte> payload)
{
    for (PeerId peer : peers())
        send(peer, type, payload);
}

void NetTransport::disconnect(PeerId peer)
{
    // A client may also abandon its handshake with disconnect(0), even before its first flush
    bool const handshake =
        _config.role == NetRole::Client && _state == ConnectionState::Connecting && peer == 0;
    auto endpoint = _endpoints.find(peer);
    auto it = endpoint == _endpoints.end() ? _peers.end() : _peers.find(endpoint->second);
    if (!handshake && (it == _peers.end() || !it->second.connected))
        return;
    if (it != _peers.end()) {
        for (int i = 0; i < _config.disconnectRedundancy; ++i) {
            Packet farewell = makePacket(PacketKind::Disconnect);
            it->second.connection.stamp(farewell, _now);
            _outbox.emplace_back(it->first, writePacket(farewell));
        }
    }
    removePeer(peer);
    if (_config.role == NetRole::Client)
        _state = ConnectionState::Disconnected;
}

std::vector<PeerId> NetTransport::peers() const
{
    std::vector<PeerId> ids;
    for (auto const &[endpoint, peer] : _peers)
        if (peer.connected)
            ids.push_back(peer.id);
    std::sort(ids.begin(), ids.end());
    return ids;
}

ConnectionState NetTransport::state() const { return _state; }

void NetTransport::receive(Endpoint const &from, std::span<const std::byte> datagram, TimePoint now)
{
    _now = std::max(_now, now);
    std::optional<Packet> packet = parsePacket(datagram);
    if (!packet)
        return;
    if (_config.role == NetRole::Server) {
        auto it = _peers.find(from);
        if (it == _peers.end())
            receiveFromUnknown(from, *packet, now);
        else
            receiveFromPeer(it->second, *packet, now);
        return;
    }
    if (_state == ConnectionState::Disconnected || !_server || !(from == *_server))
        return;
    auto it = _peers.find(from);
    if (it == _peers.end()) // nothing sent yet
        return;
    if (!it->second.connected)
        receiveWhileConnecting(it->second, *packet, now);
    else
        receiveFromPeer(it->second, *packet, now);
}

void NetTransport::receiveFromUnknown(Endpoint const &from, Packet const &packet, TimePoint now)
{
    if (packet.kind != PacketKind::ConnectRequest)
        return;
    std::optional<RejectReason> reject;
    if (packet.version != _config.protocolVersion)
        reject = RejectReason::VersionMismatch;
    else if (_peers.size() >= _config.maxPeers)
        reject = RejectReason::Full;
    if (reject) {
        Packet rejected = makePacket(PacketKind::ConnectRejected);
        rejected.reason = *reject;
        _outbox.emplace_back(from, writePacket(rejected));
        return;
    }
    PeerId const id = _nextId++;
    Peer peer{id, Connection(_config, now), true, true};
    // Return ignored: the first packet of a new connection cannot be a duplicate.
    peer.connection.receive(packet, now);
    _peers.emplace(from, std::move(peer));
    _endpoints.emplace(id, from);
    _events.connected.push_back({id});
}

void NetTransport::receiveWhileConnecting(Peer &peer, Packet const &packet, TimePoint now)
{
    switch (packet.kind) {
    case PacketKind::ConnectAccepted:
        if (!peer.connection.receive(packet, now))
            return;
        peer.connected = true;
        _state = ConnectionState::Connected;
        _events.connected.push_back({peer.id});
        return;
    case PacketKind::ConnectRejected:
        endPeer(0, packet.reason == RejectReason::Full ? DisconnectReason::Full
                                                       : DisconnectReason::VersionMismatch);
        return;
    case PacketKind::Disconnect:
        endPeer(0, DisconnectReason::Remote);
        return;
    default: // Data before ConnectAccepted included
        return;
    }
}

void NetTransport::receiveFromPeer(Peer &peer, Packet &packet, TimePoint now)
{
    bool const server = _config.role == NetRole::Server;
    bool const expected = packet.kind == PacketKind::Data || packet.kind == PacketKind::KeepAlive ||
                          packet.kind == PacketKind::Disconnect ||
                          (server && packet.kind == PacketKind::ConnectRequest) ||
                          (!server && packet.kind == PacketKind::ConnectAccepted);
    if (!expected || !peer.connection.receive(packet, now))
        return;
    PeerId const id = peer.id;
    switch (packet.kind) {
    case PacketKind::ConnectRequest: // our ConnectAccepted was lost
        peer.acceptPending = true;
        return;
    case PacketKind::Data:
        for (auto &message : packet.messages)
            _events.messages.push_back({id, message.type, std::move(message.payload)});
        return;
    case PacketKind::Disconnect:
        endPeer(id, DisconnectReason::Remote);
        return;
    default: // KeepAlive, repeated ConnectAccepted: keep the peer alive
        return;
    }
}

void NetTransport::update(TimePoint now)
{
    _now = std::max(_now, now);
    if (_config.role == NetRole::Client && _state == ConnectionState::Connecting) {
        if (_attempts >= _config.connectAttempts &&
            (!_lastRequest || now - *_lastRequest >= _config.connectRetryInterval))
            endPeer(0, DisconnectReason::ConnectFailed);
        return;
    }
    std::vector<PeerId> timedOut;
    for (auto const &[endpoint, peer] : _peers)
        if (peer.connected && peer.connection.timedOut(now))
            timedOut.push_back(peer.id);
    std::sort(timedOut.begin(), timedOut.end());
    for (PeerId id : timedOut)
        endPeer(id, DisconnectReason::Timeout);
    if (_config.logStats)
        logStats(now);
}

void NetTransport::logStats(TimePoint now)
{
    if (!_lastStatsLog)
        _lastStatsLog = now;
    if (now - *_lastStatsLog < std::chrono::seconds(1))
        return;
    float const seconds = std::chrono::duration<float>(now - *_lastStatsLog).count();
    _lastStatsLog = now;
    for (auto &[endpoint, peer] : _peers) {
        if (!peer.connected)
            continue;
        ConnectionStats const stats = peer.connection.takeStats();
        std::uint32_t const expected = stats.received + stats.missing;
        float const loss = expected == 0 ? 0.0f : 100.0f * static_cast<float>(stats.missing) /
                                                      static_cast<float>(expected);
        std::cerr << "NetworkPlugin: peer " << peer.id << " | "
                  << static_cast<int>(static_cast<float>(stats.received) / seconds)
                  << " pkt/s | loss " << loss << "% (" << stats.missing << " missing) | late "
                  << stats.late << " | dup " << stats.duplicate << "\n";
    }
}

void NetTransport::flush(TimePoint now, SendFn const &sendTo)
{
    _now = std::max(_now, now);
    if (_config.role == NetRole::Client && _state == ConnectionState::Connecting)
        flushConnectRequest(now, sendTo);
    for (auto const &[to, bytes] : _outbox)
        sendTo(to, bytes);
    _outbox.clear();
    for (auto &[to, peer] : _peers)
        if (peer.connected)
            flushPeer(to, peer, now, sendTo);
}

void NetTransport::flushConnectRequest(TimePoint now, SendFn const &sendTo)
{
    if (!_server)
        return;
    if (_attempts >= _config.connectAttempts ||
        (_lastRequest && now - *_lastRequest < _config.connectRetryInterval))
        return;
    auto it = _peers.find(*_server);
    if (it == _peers.end()) {
        it = _peers.emplace(*_server, Peer{0, Connection(_config, now), false, false}).first;
        _endpoints.emplace(0, *_server);
    }
    Packet request = makePacket(PacketKind::ConnectRequest);
    request.version = _config.protocolVersion;
    sendPacket(*_server, std::move(request), it->second, now, sendTo);
    ++_attempts;
    _lastRequest = now;
}

void NetTransport::flushPeer(Endpoint const &to, Peer &peer, TimePoint now, SendFn const &sendTo)
{
    if (peer.acceptPending) {
        peer.acceptPending = false;
        sendPacket(to, makePacket(PacketKind::ConnectAccepted), peer, now, sendTo);
    }
    std::vector<WireMessage> messages = peer.connection.takeQueued();
    for (auto const &bin : packMessages(messages)) {
        Packet data = makePacket(PacketKind::Data);
        for (std::size_t index : bin)
            data.messages.push_back(std::move(messages[index]));
        sendPacket(to, std::move(data), peer, now, sendTo);
    }
    if (peer.connection.keepAliveDue(now))
        sendPacket(to, makePacket(PacketKind::KeepAlive), peer, now, sendTo);
}

NetIncoming NetTransport::takeEvents() { return std::exchange(_events, NetIncoming{}); }

NetTransport::Peer *NetTransport::findPeer(PeerId id)
{
    auto endpoint = _endpoints.find(id);
    if (endpoint == _endpoints.end())
        return nullptr;
    auto it = _peers.find(endpoint->second);
    return it == _peers.end() ? nullptr : &it->second;
}

void NetTransport::removePeer(PeerId id)
{
    auto endpoint = _endpoints.find(id);
    if (endpoint == _endpoints.end())
        return;
    _peers.erase(endpoint->second);
    _endpoints.erase(endpoint);
}

void NetTransport::sendPacket(Endpoint const &to, Packet packet, Peer &peer, TimePoint now,
                              SendFn const &sendTo)
{
    peer.connection.stamp(packet, now);
    sendTo(to, writePacket(packet));
}

void NetTransport::endPeer(PeerId id, DisconnectReason reason)
{
    _events.disconnected.push_back({id, reason});
    removePeer(id);
    if (_config.role == NetRole::Client)
        _state = ConnectionState::Disconnected;
}
