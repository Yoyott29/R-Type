#ifndef POWERUPSCRIPT_HPP_
#define POWERUPSCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "PlayerScript.hpp"
#include "Scene.hpp"
#include "Script.hpp"

class PowerUpScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static bool isClientSide(Scene &scene);
};

inline bool PowerUpScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void PowerUpScript::onStart(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    scene.getComponent<Collider2D>(entity).onCollision = [&scene](Entity powerUp, Entity player) {
        if (!scene.isEntityAlive(player))
            return;
        PlayerScript *script = PlayerScript::from(scene, player);
        if (!script)
            return;
        script->enableDiagonalShot();
        scene.queueDestroy(powerUp);
    };
}

inline void PowerUpScript::onUpdate(Scene &, Entity &) {}

#endif /* !POWERUPSCRIPT_HPP_ */
