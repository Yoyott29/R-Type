#ifndef LOBBYSERVERSCRIPT_HPP_
#define LOBBYSERVERSCRIPT_HPP_

#include "ActionInput.hpp"
#include "Difficulty.hpp"
#include "GameScore.hpp"
#include "Lobby.hpp"
#include "MatchState.hpp"
#include "NetComponents.hpp"
#include "NetTransport.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include <optional>
#include <string>

class LobbyServerScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static std::optional<ActionInput> hostInput(Scene &scene);
    static void applyHostDifficulty(Scene &scene, const ActionInput &input);
    static void syncDifficultyMarker(Scene &scene);
    static void startMatch(Scene &scene);
};

inline void LobbyServerScript::onStart(Scene &scene, Entity &)
{
    if (Lobby::isClient(scene))
        return;
    scene.addResource<MatchState>();
    scene.addResource<Difficulty>();
}

inline void LobbyServerScript::onUpdate(Scene &scene, Entity &)
{
    if (Lobby::isClient(scene) || !isWaitingForMatch(scene))
        return;
    const std::optional<ActionInput> input = hostInput(scene);
    if (input)
        applyHostDifficulty(scene, *input);
    syncDifficultyMarker(scene);
    if (input && input->isActive(Lobby::ACTION_START))
        startMatch(scene);
}

inline std::optional<ActionInput> LobbyServerScript::hostInput(Scene &scene)
{
    const PeerId host = Lobby::hostPeer(scene);
    for (Entity entity : scene.view<Networked, RemoteInput>())
        if (scene.getComponent<Networked>(entity).owner == host)
            return scene.getComponent<RemoteInput>(entity).input;
    return std::nullopt;
}

inline void LobbyServerScript::applyHostDifficulty(Scene &scene, const ActionInput &input)
{
    const std::optional<DifficultyLevel> requested = Difficulties::decode(input);
    if (requested)
        scene.getResource<Difficulty>().level = *requested;
}

// Keeps exactly one replicated marker, the one of the current level, so clients can show it
inline void LobbyServerScript::syncDifficultyMarker(Scene &scene)
{
    const auto current = static_cast<std::size_t>(scene.getResource<Difficulty>().level);
    for (std::size_t index = 0; index < Difficulties::LEVEL_COUNT; ++index) {
        const std::string &prefab = Difficulties::TABLE[index].markerPrefab;
        const std::optional<Entity> marker = Lobby::findByPrefab(scene, prefab);
        if (index != current && marker)
            scene.queueDestroy(*marker);
        if (index == current && !marker)
            scene.getResource<PrefabsManager>().InstantiatePrefab(prefab, scene);
    }
}

inline void LobbyServerScript::startMatch(Scene &scene)
{
    scene.getResource<MatchState>().started = true;
    scene.getResource<GameScore>().value = 0;
    scene.getResource<NetTransport>().setAcceptingPeers(false);
    Lobby::clearMatchEntities(scene);
    scene.getResource<PrefabsManager>().InstantiatePrefab(Lobby::MUSIC_PREFAB, scene);
}

#endif /* !LOBBYSERVERSCRIPT_HPP_ */
