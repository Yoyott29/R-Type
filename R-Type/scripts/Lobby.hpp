#ifndef LOBBY_HPP_
#define LOBBY_HPP_

#include "BuildVariantName.hpp"
#include "MatchState.hpp"
#include "NetComponents.hpp"
#include "NetEvents.hpp"
#include "NetworkConfig.hpp"
#include "Networked.hpp"
#include "Scene.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

struct StartRequest {};

namespace Lobby
{
inline constexpr std::size_t ACTION_START = 4;
inline const std::string MUSIC_PREFAB = "StageMusic";

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
} // namespace Lobby

#endif /* !LOBBY_HPP_ */
