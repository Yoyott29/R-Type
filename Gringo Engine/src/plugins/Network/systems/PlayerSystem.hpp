/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** PlayerSystem
*/

#pragma once
#include "Events.hpp"
#include "NetEvents.hpp"
#include "NetLog.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include <map>
#include <optional>
#include <vector>

// Server: gives each connecting peer the lowest free slot s and instantiates
// NetworkConfig::playerPrefabs[s % size] as its player, owned by the peer. On a disconnect it
// destroys that player, frees the slot and broadcasts PeerLeft (even with no player prefabs)
class PlayerSystem : public System
{
  public:
    // Throws EngineError on a player prefab PrefabsManager does not have (or no PrefabsManager)
    void Start(Scene &scene) override;
    void update(Scene &scene) override;

  private:
    void spawnPlayer(Scene &scene, PeerId peer);
    void removePlayer(Scene &scene, PeerId peer, DisconnectReason reason);

    // Slot index -> the peer in it
    std::vector<std::optional<PeerId>> _slots;
    std::map<PeerId, Entity> _players;
    EventReader<PeerConnected> _connects;
    EventReader<PeerDisconnected> _disconnects;
    LogOnce _log;
};
