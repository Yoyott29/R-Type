#ifndef BULLETSCRIPT_HPP_
#define BULLETSCRIPT_HPP_

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
        scene.getResource<GameScore>().value += 100;
        scene.queueDestroy(bullet);
        EnemyScript::kill(scene, other);
    };
}

inline void BulletScript::onUpdate(Scene &, Entity &) {}

#endif /* !BULLETSCRIPT_HPP_ */
