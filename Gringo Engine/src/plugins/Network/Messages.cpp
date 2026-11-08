/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Messages
*/

#include "Messages.hpp"
#include "ByteStream.hpp"
#include "Packet.hpp"
#include <algorithm>
#include <limits>

namespace
{
constexpr std::size_t DESPAWN_HEADER_SIZE = sizeof(std::uint32_t) + sizeof(std::uint16_t);
constexpr std::uint8_t LAST_REASON = static_cast<std::uint8_t>(DisconnectReason::VersionMismatch);

// Runs body over the whole payload: a short read, a rejected field or trailing bytes give nullopt
template <typename Message, typename Body>
std::optional<Message> decodeExact(std::span<const std::byte> payload, Body body)
{
    try {
        ByteReader r(payload);
        Message m{};
        if (!body(r, m) || r.remaining() != 0)
            return std::nullopt;
        return m;
    } catch (NetError const &) {
        return std::nullopt;
    }
}
} // namespace

std::vector<std::byte> EntityStateMessage::encode() const
{
    if (prefab.size() > MAX_PREFAB_NAME)
        throw NetError("Prefab name too long for the wire");
    if (blocks.size() > std::numeric_limits<std::uint8_t>::max())
        throw NetError("Too many codec blocks for one entity");
    // tick, netId, owner, prefab length and block count, then 6 bytes of header per block
    std::size_t size =
        3 * sizeof(std::uint32_t) + sizeof(std::uint16_t) + prefab.size() + sizeof(std::uint8_t);
    for (auto const &block : blocks)
        size += sizeof(std::uint32_t) + sizeof(std::uint16_t) + block.bytes.size();
    ByteWriter w;
    w.reserve(size);
    w.write<std::uint32_t>(tick);
    w.write<std::uint32_t>(netId);
    w.write<std::uint32_t>(owner);
    w.write(prefab);
    w.write<std::uint8_t>(static_cast<std::uint8_t>(blocks.size()));
    for (auto const &block : blocks) {
        if (block.bytes.size() > std::numeric_limits<std::uint16_t>::max())
            throw NetError("Codec block too large for the wire");
        w.write<std::uint32_t>(block.codecId);
        w.write<std::uint16_t>(static_cast<std::uint16_t>(block.bytes.size()));
        w.write(std::span<const std::byte>(block.bytes));
    }
    return w.take();
}

std::vector<std::byte> InputStateMessage::encode() const
{
    ByteWriter w;
    w.write<std::uint32_t>(tick);
    w.write<std::uint32_t>(bits);
    return w.take();
}

std::vector<std::byte> ScoreUpdateMessage::encode() const
{
    ByteWriter w;
    w.write<std::uint32_t>(tick);
    w.write<std::int32_t>(score);
    return w.take();
}

std::vector<std::byte> PeerLeftMessage::encode() const
{
    ByteWriter w;
    w.write<std::uint32_t>(peer);
    w.write(reason);
    return w.take();
}

std::vector<std::vector<std::byte>>
DespawnMessage::encodeSplit(std::uint32_t tick, std::span<const std::uint32_t> netIds)
{
    const std::size_t perPayload = (MAX_PAYLOAD - DESPAWN_HEADER_SIZE) / sizeof(std::uint32_t);
    std::vector<std::vector<std::byte>> payloads;
    for (std::size_t first = 0; first < netIds.size(); first += perPayload) {
        auto chunk = netIds.subspan(first, std::min(perPayload, netIds.size() - first));
        ByteWriter w;
        w.write<std::uint32_t>(tick);
        w.write<std::uint16_t>(static_cast<std::uint16_t>(chunk.size()));
        for (std::uint32_t id : chunk)
            w.write<std::uint32_t>(id);
        payloads.push_back(w.take());
    }
    return payloads;
}

std::optional<EntityStateMessage> EntityStateMessage::decode(std::span<const std::byte> payload)
{
    return decodeExact<EntityStateMessage>(payload, [](ByteReader &r, EntityStateMessage &m) {
        m.tick = r.read<std::uint32_t>();
        m.netId = r.read<std::uint32_t>();
        m.owner = r.read<std::uint32_t>();
        m.prefab = r.readString();
        if (m.prefab.size() > MAX_PREFAB_NAME)
            return false;
        auto count = r.read<std::uint8_t>();
        m.blocks.reserve(count);
        for (std::uint8_t i = 0; i < count; ++i) {
            CodecBlock block;
            block.codecId = r.read<std::uint32_t>();
            auto bytes = r.readBytes(r.read<std::uint16_t>());
            block.bytes.assign(bytes.begin(), bytes.end());
            m.blocks.push_back(std::move(block));
        }
        return true;
    });
}

std::optional<DespawnMessage> DespawnMessage::decode(std::span<const std::byte> payload)
{
    return decodeExact<DespawnMessage>(payload, [](ByteReader &r, DespawnMessage &m) {
        m.tick = r.read<std::uint32_t>();
        auto count = r.read<std::uint16_t>();
        if (static_cast<std::size_t>(count) * sizeof(std::uint32_t) > r.remaining())
            return false;
        m.netIds.reserve(count);
        for (std::uint16_t i = 0; i < count; ++i)
            m.netIds.push_back(r.read<std::uint32_t>());
        return true;
    });
}

std::optional<InputStateMessage> InputStateMessage::decode(std::span<const std::byte> payload)
{
    return decodeExact<InputStateMessage>(payload, [](ByteReader &r, InputStateMessage &m) {
        m.tick = r.read<std::uint32_t>();
        m.bits = r.read<std::uint32_t>();
        return true;
    });
}

std::optional<PeerLeftMessage> PeerLeftMessage::decode(std::span<const std::byte> payload)
{
    return decodeExact<PeerLeftMessage>(payload, [](ByteReader &r, PeerLeftMessage &m) {
        m.peer = r.read<std::uint32_t>();
        auto reason = r.read<std::uint8_t>();
        if (reason > LAST_REASON)
            return false;
        m.reason = static_cast<DisconnectReason>(reason);
        return true;
    });
}

std::optional<ScoreUpdateMessage> ScoreUpdateMessage::decode(std::span<const std::byte> payload)
{
    return decodeExact<ScoreUpdateMessage>(payload, [](ByteReader &r, ScoreUpdateMessage &m) {
        m.tick = r.read<std::uint32_t>();
        m.score = r.read<std::int32_t>();
        return true;
    });
}
