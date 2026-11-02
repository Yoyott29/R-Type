/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SnapshotSystem
*/

#include "SnapshotSystem.hpp"
#include "NetTransport.hpp"
#include "PlayerId.hpp"
#include "Position.hpp"
#include "RTypeMessages.hpp"
#include "ServerPlayers.hpp"
#include "Type.hpp"
#include <vector>

namespace
{
// Gameplay identity only: the client picks the visuals from type, variant and playerId
SnapshotEntity buildEntity(Scene &scene, Entity entity)
{
    Position const &position = scene.getComponent<Position>(entity);
    Type const &type = scene.getComponent<Type>(entity);

    std::uint8_t playerId = rtype_protocol::NO_PLAYER;
    if (type.type == EntType::PLAYER)
        playerId = scene.getComponent<PlayerId>(entity).value;
    return {position.x, position.y, type.type, playerId, type.variant};
}
} // namespace

void SnapshotSystem::update(Scene &scene)
{
    std::uint32_t const tick = scene.getResource<ServerTick>().value;
    if (_sentOnce && tick == _lastTick)
        return;
    _sentOnce = true;
    _lastTick = tick;

    std::vector<SnapshotEntity> entities;
    entities.reserve(_entities.size());
    for (Entity entity : _entities)
        entities.push_back(buildEntity(scene, entity));

    auto &transport = scene.getResource<NetTransport>();
    for (auto const &chunk : encodeSnapshot(tick, entities))
        transport.broadcast(static_cast<std::uint16_t>(RTypeMessage::Snapshot), chunk);
}
