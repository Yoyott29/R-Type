/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Packet
*/

#include "Packet.hpp"
#include "ByteStream.hpp"

std::size_t encodedSize(WireMessage const &message)
{
    return MESSAGE_HEADER_SIZE + message.payload.size();
}

std::vector<std::byte> writePacket(Packet const &packet)
{
    std::size_t size = PACKET_HEADER_SIZE + 2; // largest control body
    for (auto const &m : packet.messages)
        size += encodedSize(m);
    ByteWriter w;
    w.reserve(size);
    w.write<std::uint16_t>(PROTOCOL_ID);
    w.write(packet.kind);
    w.write<std::uint16_t>(packet.seq);
    switch (packet.kind) {
    case PacketKind::ConnectRequest:
        w.write<std::uint16_t>(packet.version);
        break;
    case PacketKind::ConnectRejected:
        w.write(packet.reason);
        break;
    case PacketKind::Data:
        for (auto const &m : packet.messages) {
            w.write<std::uint16_t>(m.type);
            w.write<std::uint16_t>(static_cast<std::uint16_t>(m.payload.size()));
            w.write(std::span<const std::byte>(m.payload));
        }
        break;
    default:
        break;
    }
    return w.take();
}

static std::optional<Packet> parseBody(ByteReader &r, Packet &p)
{
    switch (p.kind) {
    case PacketKind::ConnectRequest:
        if (r.remaining() != 2)
            return std::nullopt;
        p.version = r.read<std::uint16_t>();
        return std::move(p);
    case PacketKind::ConnectRejected: {
        if (r.remaining() != 1)
            return std::nullopt;
        auto reason = r.read<std::uint8_t>();
        if (reason > static_cast<std::uint8_t>(RejectReason::VersionMismatch))
            return std::nullopt;
        p.reason = static_cast<RejectReason>(reason);
        return std::move(p);
    }
    case PacketKind::Data:
        while (r.remaining() > 0) {
            WireMessage m{r.read<std::uint16_t>(), {}};
            auto length = r.read<std::uint16_t>();
            auto payload = r.readBytes(length);
            m.payload.assign(payload.begin(), payload.end());
            p.messages.push_back(std::move(m));
        }
        return std::move(p);
    default:
        if (r.remaining() != 0)
            return std::nullopt;
        return std::move(p);
    }
}

std::optional<Packet> parsePacket(std::span<const std::byte> datagram)
{
    if (datagram.size() > MAX_DATAGRAM_SIZE || datagram.size() < PACKET_HEADER_SIZE)
        return std::nullopt;
    try {
        ByteReader r{datagram};
        if (r.read<std::uint16_t>() != PROTOCOL_ID)
            return std::nullopt;
        auto kind = r.read<std::uint8_t>();
        if (kind > static_cast<std::uint8_t>(PacketKind::Disconnect))
            return std::nullopt;
        Packet p{static_cast<PacketKind>(kind), 0, 0, RejectReason::Full, {}};
        p.seq = r.read<std::uint16_t>();
        return parseBody(r, p);
    } catch (NetError const &) {
        return std::nullopt;
    }
}

std::vector<std::vector<std::size_t>> packMessages(std::span<WireMessage const> messages)
{
    std::vector<std::vector<std::size_t>> bins;
    std::vector<std::size_t> used;
    for (std::size_t i = 0; i < messages.size(); ++i) {
        std::size_t size = encodedSize(messages[i]);
        std::size_t target = bins.size();
        for (std::size_t b = 0; b < bins.size(); ++b) {
            if (used[b] + size <= MAX_DATAGRAM_SIZE) {
                target = b;
                break;
            }
        }
        if (target == bins.size()) {
            bins.emplace_back();
            used.push_back(PACKET_HEADER_SIZE);
        }
        bins[target].push_back(i);
        used[target] += size;
    }
    return bins;
}
