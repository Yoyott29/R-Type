#ifndef LOBBY_HPP_
#define LOBBY_HPP_

#include "BuildVariantName.hpp"
#include "GameScore.hpp"
#include "MatchState.hpp"
#include "NetComponents.hpp"
#include "NetEvents.hpp"
#include "NetTransport.hpp"
#include "NetworkConfig.hpp"
#include "Networked.hpp"
#include "Scene.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

struct StartRequest {
};

namespace Lobby
{
inline constexpr std::size_t ACTION_START = 4;
inline const std::string MUSIC_PREFAB = "StageMusic";
inline constexpr float HIDDEN_OFFSET_Y = 5000.0f;

// Moves a menu (and its children) off screen or back. False when it already was in that state
inline bool setHidden(Scene &scene, Entity menu, bool &state, bool hidden)
{
    if (hidden == state)
        return false;
    state = hidden;
    scene.getComponent<Transform>(menu).y += hidden ? HIDDEN_OFFSET_Y : -HIDDEN_OFFSET_Y;
    return true;
}

inline bool isClient(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline bool isPlayer(Scene &scene, const Networked &networked)
{
    const std::vector<std::string> &prefabs = scene.getResource<NetworkConfig>().playerPrefabs;
    return networked.owner != NO_PEER &&
           std::find(prefabs.begin(), prefabs.end(), networked.prefab) != prefabs.end();
}

inline PeerId hostPeer(Scene &scene)
{
    PeerId host = NO_PEER;
    for (Entity entity : scene.view<Networked>()) {
        const Networked &networked = scene.getComponent<Networked>(entity);
        if (isPlayer(scene, networked))
            host = std::min(host, networked.owner);
    }
    return host;
}

inline std::optional<Entity> findByPrefab(Scene &scene, const std::string &prefab)
{
    for (Entity entity : scene.view<Networked>())
        if (scene.getComponent<Networked>(entity).prefab == prefab)
            return entity;
    return std::nullopt;
}

inline void clearMatchEntities(Scene &scene)
{
    for (Entity entity : scene.view<Networked>()) {
        const Networked &networked = scene.getComponent<Networked>(entity);
        if (networked.mode == NetMode::Replicated && !isPlayer(scene, networked))
            scene.queueDestroy(entity);
    }
}

inline void endMatch(Scene &scene)
{
    scene.getResource<MatchState>().started = false;
    scene.getResource<GameScore>().value = 0;
    scene.getResource<NetTransport>().setAcceptingPeers(true);
    clearMatchEntities(scene);
}
} // namespace Lobby

#endif /* !LOBBY_HPP_ */
