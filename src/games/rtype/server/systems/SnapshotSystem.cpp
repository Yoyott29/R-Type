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
#include "Sprite.hpp"
#include "Type.hpp"
#include <type_traits>
#include <variant>
#include <vector>

namespace
{
SnapshotEntity buildEntity(Scene &scene, Entity entity)
{
    Position const &position = scene.getComponent<Position>(entity);
    Type const &type = scene.getComponent<Type>(entity);
    Sprite const &sprite = scene.getComponent<Sprite>(entity);

    std::uint8_t playerId = rtype_protocol::NO_PLAYER;
    if (type.type == EntType::PLAYER)
        playerId = scene.getComponent<PlayerId>(entity).value;
    std::uint8_t variant = rtype_protocol::DEFAULT_VARIANT;
    if (sprite.texturePath == "Assets/Bydos2.png")
        variant = rtype_protocol::ENEMY_VARIANT_2;

    float width = 0.0f;
    float height = 0.0f;
    std::uint8_t shape = rtype_protocol::BOX_SHAPE;
    std::visit(
        [&](auto const &value) {
            using Shape = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<Shape, BoxShape>) {
                width = value.width;
                height = value.height;
            } else {
                width = value.radius;
                height = value.radius;
                shape = rtype_protocol::CIRCLE_SHAPE;
            }
        },
        sprite.shape);
    return {position.x,
            position.y,
            type.type,
            playerId,
            variant,
            width,
            height,
            shape,
            static_cast<std::int8_t>(sprite.drawOrder),
            {sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a}};
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
