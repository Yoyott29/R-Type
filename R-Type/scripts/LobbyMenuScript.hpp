#ifndef LOBBYMENUSCRIPT_HPP_
#define LOBBYMENUSCRIPT_HPP_

#include "ActionInput.hpp"
#include "Lobby.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Settings.hpp"

class LobbyMenuScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    bool _hidden = false;
};

inline void LobbyMenuScript::onStart(Scene &, Entity &) {}

inline void LobbyMenuScript::onUpdate(Scene &scene, Entity &entity)
{
    if (!Lobby::isClient(scene))
        return;
    const bool inMatch = Lobby::findByPrefab(scene, Lobby::MUSIC_PREFAB).has_value();
    const bool hidden = inMatch || scene.hasResource<SettingsOpen>();
    if (Lobby::setHidden(scene, entity, _hidden, hidden) && hidden)
        scene.removeResource<StartRequest>();
    if (hidden)
        return;
    if (scene.hasResource<StartRequest>() && scene.hasResource<ActionInput>())
        scene.getResource<ActionInput>().active.set(Lobby::ACTION_START);
}

#endif /* !LOBBYMENUSCRIPT_HPP_ */
