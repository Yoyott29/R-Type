/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** MirrorSystem
*/

#pragma once
#include "Events.hpp"
#include "Messages.hpp"
#include "NetEvents.hpp"
#include "NetLog.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include <cstdint>
#include <unordered_map>

// Client ticks without an EntityState after which a mirror is destroyed
inline constexpr std::uint32_t MIRROR_STALE_TICKS = 30;

// Client: turns the server's EntityState, Despawn and PeerLeft messages into mirror entities,
// destroys the stale ones, and all of them when the server disconnects
class MirrorSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    // True while entity is alive and still the mirror of netId (its id may have been reused)
    bool isKnown(Scene &scene, std::uint32_t netId, Entity entity) const;
    // Drops the entries whose mirror is no longer known
    void forgetDestroyed(Scene &scene);
    void handleState(Scene &scene, EntityStateMessage const &message, std::uint32_t now);
    // Instantiates the prefab as the mirror of netId, unless capped or the prefab is unknown
    void spawn(Scene &scene, EntityStateMessage const &message, std::uint32_t now);
    void applyState(Scene &scene, Entity entity, EntityStateMessage const &message,
                    std::uint32_t now);
    void destroyMirror(Scene &scene, std::uint32_t netId);
    void destroyAll(Scene &scene);
    // True if the tick is not older than the highest one seen, which it then becomes
    bool inOrder(std::uint32_t tick);

    std::unordered_map<std::uint32_t, Entity> _mirrors;
    std::uint32_t _highestTick = 0;
    EventReader<PeerDisconnected> _disconnects;
    LogOnce _log;
};
