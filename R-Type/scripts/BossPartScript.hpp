#ifndef BOSSPARTSCRIPT_HPP_
#define BOSSPARTSCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "PlayerScript.hpp"
#include "Scene.hpp"
#include "Script.hpp"

class BossPartScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static bool isClientSide(Scene &scene);
};

inline bool BossPartScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void BossPartScript::onStart(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    scene.getComponent<Collider2D>(entity).onCollision = [&scene](Entity, Entity other) {
        if (!scene.isEntityAlive(other))
            return;
        if (scene.getComponent<Collider2D>(other).layer != PlayerScript::PLAYER_LAYER)
            return;
        PlayerScript::kill(scene, other);
    };
}

inline void BossPartScript::onUpdate(Scene &, Entity &) {}

#endif /* !BOSSPARTSCRIPT_HPP_ */
