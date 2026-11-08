/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** MirrorSystem
*/

#include "MirrorSystem.hpp"
#include "ChildOf.hpp"
#include "DecodeOrLog.hpp"
#include "GameClock.hpp"
#include "GameScore.hpp"
#include "NetComponents.hpp"
#include "Networked.hpp"
#include "Packet.hpp"
#include "Replication.hpp"
#include "ReservedInbox.hpp"
#include "TryInstantiate.hpp"
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

bool MirrorSystem::isKnown(Scene &scene, std::uint32_t netId, Entity entity) const
{
    return scene.hasComponent<Mirror>(entity) && scene.getComponent<Mirror>(entity).netId == netId;
}

bool MirrorSystem::inOrder(std::uint32_t tick)
{
    if (tick < _highestTick)
        return false;
    _highestTick = tick;
    return true;
}

void MirrorSystem::forgetDestroyed(Scene &scene)
{
    std::erase_if(_mirrors,
                  [&](auto const &pair) { return !isKnown(scene, pair.first, pair.second); });
}

void MirrorSystem::update(Scene &scene)
{
    auto const now = static_cast<std::uint32_t>(scene.getResource<GameClock>().tick);
    for (NetMessage const &message : scene.getResource<ReservedInbox>().messages) {
        switch (static_cast<ReservedType>(message.type)) {
        case ReservedType::EntityState:
            if (auto state = decodeOrLog<EntityStateMessage>(_log, message, "EntityState"))
                if (inOrder(state->tick))
                    handleState(scene, *state, now);
            break;
        case ReservedType::Despawn:
            if (auto gone = decodeOrLog<DespawnMessage>(_log, message, "Despawn"))
                if (inOrder(gone->tick))
                    for (std::uint32_t netId : gone->netIds)
                        destroyMirror(scene, netId);
            break;
        case ReservedType::PeerLeft:
            if (auto left = decodeOrLog<PeerLeftMessage>(_log, message, "PeerLeft"))
                scene.getResource<Events<PeerLeft>>().send(PeerLeft{left->peer, left->reason});
            break;
        case ReservedType::ScoreUpdate:
            if (auto score = decodeOrLog<ScoreUpdateMessage>(_log, message, "ScoreUpdate"))
                if (inOrder(score->tick))
                    scene.getResource<GameScore>().value = score->score;
            break;
        default: {
            std::string const type = std::to_string(message.type);
            _log("unexpected" + type, "ignored reserved message type " + type);
            break;
        }
        }
    }

    // One pass: forgets the mirrors a script destroyed and destroys the stale ones
    std::erase_if(_mirrors, [&](auto const &pair) {
        auto const &[netId, entity] = pair;
        if (!isKnown(scene, netId, entity))
            return true;
        if (now - scene.getComponent<Mirror>(entity).lastSeen <= MIRROR_STALE_TICKS)
            return false;
        scene.destroyEntity(entity);
        return true;
    });

    bool disconnected = false;
    _disconnects.read(scene, [&](PeerDisconnected const &) { disconnected = true; });
    if (disconnected) {
        destroyAll(scene);
        _highestTick = 0;
    }
}

void MirrorSystem::handleState(Scene &scene, EntityStateMessage const &message, std::uint32_t now)
{
    auto const found = _mirrors.find(message.netId);
    if (found != _mirrors.end() && isKnown(scene, message.netId, found->second)) {
        applyState(scene, found->second, message, now);
        return;
    }
    // A stale entry (destroyed mirror, maybe a reused id) is never touched again
    if (found != _mirrors.end())
        _mirrors.erase(found);
    spawn(scene, message, now);
}

void MirrorSystem::spawn(Scene &scene, EntityStateMessage const &message, std::uint32_t now)
{
    // A script may have destroyed some since the last pass: count only the living ones
    if (_mirrors.size() >= MAX_MIRRORS)
        forgetDestroyed(scene);
    if (_mirrors.size() >= MAX_MIRRORS) {
        _log("mirrorCap", "mirror limit reached, entities not shown");
        return;
    }
    std::optional<Entity> const instance = tryInstantiate(scene, message.prefab, _log, "mirror");
    if (!instance)
        return;
    Entity const entity = *instance;
    // A mirror is a top-level entity: the server sends world positions
    if (scene.hasComponent<ChildOf>(entity))
        scene.removeComponent<ChildOf>(entity);
    // Added right away so the next SideFilterSystem keeps it
    scene.addComponent(entity, Mirror{message.netId, now});
    _mirrors[message.netId] = entity;
    applyState(scene, entity, message, now);
}

void MirrorSystem::applyState(Scene &scene, Entity entity, EntityStateMessage const &message,
                              std::uint32_t now)
{
    if (scene.hasResource<CodecRegistry>()) {
        auto const &codecs = scene.getResource<CodecRegistry>().codecs;
        std::unordered_set<CodecId> present;
        // Unknown ids (server-only codecs) are skipped; their len already bounded them
        for (CodecBlock const &block : message.blocks) {
            present.insert(block.codecId);
            auto const codec = codecs.find(block.codecId);
            if (codec != codecs.end() && !codec->second.apply(scene, entity, block.bytes))
                _log("codec" + codec->second.name,
                     "skipped a malformed " + codec->second.name + " block");
        }
        for (auto const &[id, codec] : codecs)
            if (!present.contains(id))
                codec.remove(scene, entity);
    }
    if (scene.hasComponent<Networked>(entity)) {
        scene.getComponent<Networked>(entity).owner = message.owner;
    } else {
        Networked networked;
        networked.mode = NetMode::Replicated;
        networked.prefab = message.prefab;
        networked.owner = message.owner;
        scene.addComponent(entity, networked);
    }
    scene.getComponent<Mirror>(entity).lastSeen = now;
}

void MirrorSystem::destroyMirror(Scene &scene, std::uint32_t netId)
{
    auto const found = _mirrors.find(netId);
    if (found == _mirrors.end())
        return;
    if (isKnown(scene, netId, found->second))
        scene.destroyEntity(found->second);
    _mirrors.erase(found);
}

void MirrorSystem::destroyAll(Scene &scene)
{
    for (auto const &[netId, entity] : _mirrors)
        if (isKnown(scene, netId, entity))
            scene.destroyEntity(entity);
    _mirrors.clear();
}
