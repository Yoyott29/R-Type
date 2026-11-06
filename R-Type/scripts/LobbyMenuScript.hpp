#ifndef LOBBYMENUSCRIPT_HPP_
#define LOBBYMENUSCRIPT_HPP_

#include "ActionInput.hpp"
#include "Lobby.hpp"
#include "Scene.hpp"
#include "Script.hpp"

class LobbyMenuScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
};

inline void LobbyMenuScript::onStart(Scene &, Entity &) {}

inline void LobbyMenuScript::onUpdate(Scene &scene, Entity &entity)
{
    if (!Lobby::isClient(scene))
        return;
    if (Lobby::findByPrefab(scene, Lobby::MUSIC_PREFAB)) {
        scene.queueDestroy(entity);
        return;
    }
    if (scene.hasResource<StartRequest>() && scene.hasResource<ActionInput>())
        scene.getResource<ActionInput>().active.set(Lobby::ACTION_START);
}

#endif /* !LOBBYMENUSCRIPT_HPP_ */
