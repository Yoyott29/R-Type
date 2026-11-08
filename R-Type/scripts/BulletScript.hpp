#ifndef BULLETSCRIPT_HPP_
#define BULLETSCRIPT_HPP_

#include "BossState.hpp"
#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "EnemyScript.hpp"
#include "GameScore.hpp"
#include "Scene.hpp"
#include "Script.hpp"

class BulletScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static bool isClientSide(Scene &scene);
};

inline bool BulletScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void BulletScript::onStart(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    scene.getComponent<Collider2D>(entity).onCollision = [&scene](Entity bullet, Entity other) {
        if (!scene.isEntityAlive(other))
            return;
        scene.queueDestroy(bullet);
        if (scene.getComponent<Collider2D>(other).layer == BOSS_LAYER) {
            hitBoss(scene, 1);
            return;
        }
        scene.getResource<GameScore>().value += 100;
        EnemyScript::kill(scene, other);
    };
}

inline void BulletScript::onUpdate(Scene &, Entity &) {}

#endif /* !BULLETSCRIPT_HPP_ */
