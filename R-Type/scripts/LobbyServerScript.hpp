#ifndef LOBBYSERVERSCRIPT_HPP_
#define LOBBYSERVERSCRIPT_HPP_

#include "GameScore.hpp"
#include "Lobby.hpp"
#include "MatchState.hpp"
#include "NetComponents.hpp"
#include "NetTransport.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include <optional>

class LobbyServerScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static bool hostPressedStart(Scene &scene);
    static void startMatch(Scene &scene);
};

inline void LobbyServerScript::onStart(Scene &scene, Entity &)
{
    if (Lobby::isClient(scene))
        return;
    scene.addResource<MatchState>();
}

inline void LobbyServerScript::onUpdate(Scene &scene, Entity &)
{
    if (Lobby::isClient(scene) || !isWaitingForMatch(scene))
        return;
    if (hostPressedStart(scene))
        startMatch(scene);
}

inline bool LobbyServerScript::hostPressedStart(Scene &scene)
{
    const PeerId host = Lobby::hostPeer(scene);
    for (Entity entity : scene.view<Networked, RemoteInput>())
        if (scene.getComponent<Networked>(entity).owner == host &&
            scene.getComponent<RemoteInput>(entity).input.isActive(Lobby::ACTION_START))
            return true;
    return false;
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
