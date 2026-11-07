#ifndef ENEMYBULLETSCRIPT_HPP_
#define ENEMYBULLETSCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "PlayerScript.hpp"
#include "Scene.hpp"
#include "Script.hpp"

class EnemyBulletScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static bool isClientSide(Scene &scene);
};

inline bool EnemyBulletScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void EnemyBulletScript::onStart(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    scene.getComponent<Collider2D>(entity).onCollision = [&scene](Entity bullet, Entity player) {
        if (!scene.isEntityAlive(player))
            return;
        scene.queueDestroy(bullet);
        PlayerScript::kill(scene, player);
    };
}

inline void EnemyBulletScript::onUpdate(Scene &, Entity &) {}

#endif /* !ENEMYBULLETSCRIPT_HPP_ */
