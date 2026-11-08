/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ReplicationSystem
*/

#include "ReplicationSystem.hpp"
#include "ByteStream.hpp"
#include "GameClock.hpp"
#include "GameScore.hpp"
#include "Messages.hpp"
#include "NetComponents.hpp"
#include "NetTransport.hpp"
#include "Networked.hpp"
#include "Packet.hpp"
#include "Replication.hpp"
#include <string>
#include <vector>

void ReplicationSystem::assignNetIds(Scene &scene)
{
    for (Entity entity : scene.view<Networked>())
        if (scene.getComponent<Networked>(entity).mode == NetMode::Replicated &&
            !scene.hasComponent<NetId>(entity))
            scene.addComponent(entity, NetId{_nextNetId++});
}

std::optional<std::vector<std::byte>>
ReplicationSystem::tryEncode(EntityStateMessage const &message)
{
    std::string reason;
    try {
        std::vector<std::byte> payload = message.encode();
        if (payload.size() <= MAX_PAYLOAD)
            return payload;
        reason = std::to_string(payload.size()) + " bytes is over the message limit";
    } catch (NetError const &error) {
        reason = error.what();
    }
    std::string const netId = std::to_string(message.netId);
    _log("oversized" + message.prefab,
         message.prefab + " entity " + netId + " not replicated: " + reason);
    return std::nullopt;
}

void ReplicationSystem::update(Scene &scene)
{
    assignNetIds(scene);
    std::uint64_t const tick = scene.getResource<GameClock>().tick;
    std::uint64_t const snapshot = tick / SNAPSHOT_TICK_DIVISOR;
    if (_lastSnapshot == snapshot)
        return;
    _lastSnapshot = snapshot;
    auto const wireTick = static_cast<std::uint32_t>(tick);

    std::vector<Entity> entities;
    std::vector<EntityStateMessage> messages;
    // Every replicated entity, even one skipped below, so it is not despawned
    std::unordered_set<std::uint32_t> current;
    for (Entity entity : scene.view<Networked, NetId>()) {
        Networked const &networked = scene.getComponent<Networked>(entity);
        if (networked.mode != NetMode::Replicated)
            continue;
        std::uint32_t const netId = scene.getComponent<NetId>(entity).value;
        entities.push_back(entity);
        messages.push_back(
            EntityStateMessage{wireTick, netId, networked.owner, networked.prefab, {}});
        current.insert(netId);
    }
    // Codecs outside, entities inside: one walk of the registry per snapshot
    if (scene.hasResource<CodecRegistry>())
        for (auto const &[id, codec] : scene.getResource<CodecRegistry>().codecs)
            for (std::size_t i = 0; i < entities.size(); ++i)
                if (codec.has(scene, entities[i])) {
                    ByteWriter writer;
                    codec.write(scene, entities[i], writer);
                    messages[i].blocks.push_back(CodecBlock{id, writer.take()});
                }

    auto &transport = scene.getResource<NetTransport>();
    for (EntityStateMessage const &message : messages)
        if (auto payload = tryEncode(message))
            transport.broadcastReserved(ReservedType::EntityState, *payload);

    for (std::uint32_t netId : _previous)
        if (!current.contains(netId))
            _despawns[netId] = DESPAWN_REPEATS;
    // One sweep: a netId back in the snapshot stops being despawned, the others count down
    std::vector<std::uint32_t> gone;
    std::erase_if(_despawns, [&](auto &countdown) {
        if (current.contains(countdown.first))
            return true;
        gone.push_back(countdown.first);
        return --countdown.second == 0;
    });
    _previous = std::move(current);
    for (auto const &payload : DespawnMessage::encodeSplit(wireTick, gone))
        transport.broadcastReserved(ReservedType::Despawn, payload);
    transport.broadcastReserved(
        ReservedType::ScoreUpdate,
        ScoreUpdateMessage{wireTick, scene.getResource<GameScore>().value}.encode());
}
