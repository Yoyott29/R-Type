#ifndef LOBBYMENUSCRIPT_HPP_
#define LOBBYMENUSCRIPT_HPP_

#include "ActionInput.hpp"
#include "Lobby.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"

class LobbyMenuScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static constexpr float HIDDEN_OFFSET_Y = 5000.0f;

    void setHidden(Scene &scene, Entity entity, bool hidden);

    bool _hidden = false;
};

inline void LobbyMenuScript::onStart(Scene &, Entity &) {}

inline void LobbyMenuScript::onUpdate(Scene &scene, Entity &entity)
{
    if (!Lobby::isClient(scene))
        return;
    const bool inMatch = Lobby::findByPrefab(scene, Lobby::MUSIC_PREFAB).has_value();
    setHidden(scene, entity, inMatch);
    if (inMatch)
        return;
    if (scene.hasResource<StartRequest>() && scene.hasResource<ActionInput>())
        scene.getResource<ActionInput>().active.set(Lobby::ACTION_START);
}

inline void LobbyMenuScript::setHidden(Scene &scene, Entity entity, bool hidden)
{
    if (hidden == _hidden)
        return;
    _hidden = hidden;
    Transform &position = scene.getComponent<Transform>(entity);
    if (!hidden) {
        position.y -= HIDDEN_OFFSET_Y;
        return;
    }
    position.y += HIDDEN_OFFSET_Y;
    if (scene.hasResource<StartRequest>())
        scene.removeResource<StartRequest>();
}

#endif /* !LOBBYMENUSCRIPT_HPP_ */
