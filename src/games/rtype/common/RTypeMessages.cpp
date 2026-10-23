/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RTypeMessages
*/

#include "RTypeMessages.hpp"
#include "ByteStream.hpp"
#include "rtype_protocol.hpp"
#include <algorithm>

static void expectEnd(ByteReader const &reader)
{
    if (reader.remaining() != 0)
        throw NetError("Trailing bytes after the message");
}

std::vector<std::byte> encodeWelcome(WelcomeMessage message)
{
    ByteWriter writer;
    writer.write<std::uint8_t>(message.playerId);
    return writer.take();
}

WelcomeMessage decodeWelcome(std::span<const std::byte> bytes)
{
    ByteReader reader(bytes);
    WelcomeMessage message{reader.read<std::uint8_t>()};
    expectEnd(reader);
    if (message.playerId >= rtype_protocol::MAX_PLAYERS)
        throw NetError("Welcome player id out of range");
    return message;
}

std::vector<std::byte> encodeInput(InputMessage message)
{
    ByteWriter writer;
    writer.write<std::uint32_t>(message.tick);
    writer.write<std::uint8_t>(message.buttons);
    return writer.take();
}

InputMessage decodeInput(std::span<const std::byte> bytes)
{
    ByteReader reader(bytes);
    InputMessage message{};
    message.tick = reader.read<std::uint32_t>();
    message.buttons = reader.read<std::uint8_t>();
    expectEnd(reader);
    if (message.buttons & rtype_protocol::INPUT_UNUSED_BITS)
        throw NetError("Input uses reserved bits");
    return message;
}

static void writeEntity(ByteWriter &writer, SnapshotEntity const &entity)
{
    writer.write<float>(entity.x);
    writer.write<float>(entity.y);
    writer.write<std::uint8_t>(static_cast<std::uint8_t>(entity.type));
    writer.write<std::uint8_t>(entity.playerId);
    writer.write<std::uint8_t>(entity.variant);
    writer.write<float>(entity.width);
    writer.write<float>(entity.height);
    writer.write<std::uint8_t>(entity.shape);
    writer.write<std::int8_t>(entity.drawOrder);
    for (std::uint8_t channel : entity.color)
        writer.write<std::uint8_t>(channel);
}

static SnapshotEntity readEntity(ByteReader &reader)
{
    SnapshotEntity entity{};
    entity.x = reader.read<float>();
    entity.y = reader.read<float>();
    auto type = reader.read<std::uint8_t>();
    if (type > static_cast<std::uint8_t>(OTHER))
        throw NetError("Snapshot entity type out of range");
    entity.type = static_cast<EntType>(type);
    entity.playerId = reader.read<std::uint8_t>();
    entity.variant = reader.read<std::uint8_t>();
    entity.width = reader.read<float>();
    entity.height = reader.read<float>();
    entity.shape = reader.read<std::uint8_t>();
    entity.drawOrder = reader.read<std::int8_t>();
    for (std::uint8_t &channel : entity.color)
        channel = reader.read<std::uint8_t>();
    if (entity.shape > rtype_protocol::CIRCLE_SHAPE)
        throw NetError("Snapshot entity shape out of range");
    if (entity.type == PLAYER && entity.playerId >= rtype_protocol::MAX_PLAYERS)
        throw NetError("Snapshot player id out of range");
    return entity;
}

std::vector<std::vector<std::byte>> encodeSnapshot(std::uint32_t tick,
                                                   std::span<const SnapshotEntity> entities)
{
    std::vector<std::vector<std::byte>> chunks;
    std::size_t offset = 0;
    while (offset < entities.size()) {
        std::size_t count = std::min(SNAPSHOT_ENTITIES_PER_CHUNK, entities.size() - offset);
        ByteWriter writer;
        writer.reserve(SNAPSHOT_HEADER_BYTES + count * SNAPSHOT_ENTITY_BYTES);
        writer.write<std::uint32_t>(tick);
        writer.write<std::uint16_t>(static_cast<std::uint16_t>(count));
        for (std::size_t i = 0; i < count; ++i)
            writeEntity(writer, entities[offset + i]);
        chunks.push_back(writer.take());
        offset += count;
    }
    return chunks;
}

SnapshotMessage decodeSnapshot(std::span<const std::byte> bytes)
{
    ByteReader reader(bytes);
    SnapshotMessage message{};
    message.tick = reader.read<std::uint32_t>();
    auto count = reader.read<std::uint16_t>();
    if (reader.remaining() != static_cast<std::size_t>(count) * SNAPSHOT_ENTITY_BYTES)
        throw NetError("Snapshot size does not match its entity count");
    message.entities.reserve(count);
    for (std::uint16_t i = 0; i < count; ++i)
        message.entities.push_back(readEntity(reader));
    return message;
}
