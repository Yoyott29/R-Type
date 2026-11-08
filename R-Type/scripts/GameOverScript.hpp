#ifndef GAMEOVERSCRIPT_HPP_
#define GAMEOVERSCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "GameScore.hpp"
#include "Lobby.hpp"
#include "MatchState.hpp"
#include "NetTransport.hpp"
#include "Networked.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include <cstdint>

class GameOverScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static constexpr std::uint32_t PLAYER_LAYER = 1;
    static constexpr std::uint32_t ENEMY_LAYER = 2;

    static bool isClientSide(Scene &scene);
    static bool hasAlivePlayer(Scene &scene);
    static void backToLobby(Scene &scene);
};

inline bool GameOverScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline bool GameOverScript::hasAlivePlayer(Scene &scene)
{
    for (Entity entity : scene.view<Transform, Collider2D>())
        if (scene.getComponent<Collider2D>(entity).layer == PLAYER_LAYER)
            return true;
    return false;
}

inline void GameOverScript::backToLobby(Scene &scene)
{
    for (Entity entity : scene.view<Transform, Collider2D>())
        if (scene.getComponent<Collider2D>(entity).layer == ENEMY_LAYER)
            scene.queueDestroy(entity);
    Lobby::endMatch(scene);
}

inline void GameOverScript::onStart(Scene &, Entity &) {}

inline void GameOverScript::onUpdate(Scene &scene, Entity &)
{
    if (isClientSide(scene) || isWaitingForMatch(scene))
        return;
    if (hasAlivePlayer(scene))
        return;
    backToLobby(scene);
}

#endif /* !GAMEOVERSCRIPT_HPP_ */
