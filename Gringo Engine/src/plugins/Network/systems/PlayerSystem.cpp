/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** PlayerSystem
*/

#include "PlayerSystem.hpp"
#include "EngineError.hpp"
#include "Messages.hpp"
#include "NetTransport.hpp"
#include "NetworkConfig.hpp"
#include "Networked.hpp"
#include "Packet.hpp"
#include "PrefabsManager.hpp"
#include "TryInstantiate.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>

void PlayerSystem::Start(Scene &scene)
{
    auto const &config = scene.getResource<NetworkConfig>();
    _slots.assign(config.maxPeers, std::nullopt);
    if (config.playerPrefabs.empty())
        return;
    if (!scene.hasResource<PrefabsManager>())
        throw EngineError("Network: unknown player prefab " + config.playerPrefabs.front());
    auto const &manager = scene.getResource<PrefabsManager>();
    for (std::string const &name : config.playerPrefabs)
        if (!manager.hasPrefab(name))
            throw EngineError("Network: unknown player prefab " + name);
}

void PlayerSystem::update(Scene &scene)
{
    // Connections first: a peer that joined and left in the same frame ends with no player
    _connects.read(scene, [&](PeerConnected const &event) { spawnPlayer(scene, event.peer); });
    _disconnects.read(scene, [&](PeerDisconnected const &event) {
        removePlayer(scene, event.peer, event.reason);
    });
}

void PlayerSystem::spawnPlayer(Scene &scene, PeerId peer)
{
    auto const &prefabs = scene.getResource<NetworkConfig>().playerPrefabs;
    if (prefabs.empty())
        return;
    auto const free = std::find(_slots.begin(), _slots.end(), std::nullopt);
    if (free == _slots.end()) {
        _log("slotsFull", "no free player slot, peer " + std::to_string(peer) + " has no player");
        return;
    }
    auto const slot = static_cast<std::size_t>(free - _slots.begin());
    std::string const &name = prefabs[slot % prefabs.size()];
    std::optional<Entity> const instance = tryInstantiate(scene, name, _log, "player");
    if (!instance)
        return;
    Entity const entity = *instance;
    if (!scene.hasComponent<Networked>(entity)) {
        Networked networked;
        networked.mode = NetMode::Replicated;
        networked.prefab = name;
        scene.addComponent(entity, networked);
    }
    Networked &networked = scene.getComponent<Networked>(entity);
    if (networked.prefab.empty())
        networked.prefab = name;
    networked.owner = peer;
    *free = peer;
    _players[peer] = entity;
}

void PlayerSystem::removePlayer(Scene &scene, PeerId peer, DisconnectReason reason)
{
    auto const player = _players.find(peer);
    if (player != _players.end()) {
        Entity const entity = player->second;
        // A script may have destroyed it and its id gone to another entity: touch only our own
        if (scene.isEntityAlive(entity) && scene.hasComponent<Networked>(entity) &&
            scene.getComponent<Networked>(entity).owner == peer)
            scene.destroyEntity(entity);
        _players.erase(player);
    }
    auto const slot = std::find(_slots.begin(), _slots.end(), peer);
    if (slot != _slots.end())
        slot->reset();
    scene.getResource<NetTransport>().broadcastReserved(ReservedType::PeerLeft,
                                                        PeerLeftMessage{peer, reason}.encode());
}
