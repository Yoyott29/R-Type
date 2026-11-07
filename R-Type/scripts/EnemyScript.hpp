#ifndef ENEMYSCRIPT_HPP_
#define ENEMYSCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "PlayerScript.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include "Velocity.hpp"
#include <cmath>
#include <cstdint>

class EnemyScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static constexpr std::uint32_t PLAYER_LAYER = 1;

    static bool isClientSide(Scene &scene);
};

inline bool EnemyScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void EnemyScript::onStart(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    scene.getComponent<Collider2D>(entity).onCollision = [&scene](Entity, Entity other) {
        if (!scene.isEntityAlive(other))
            return;
        if (scene.getComponent<Collider2D>(other).layer != PLAYER_LAYER)
            return;
        PlayerScript::kill(scene, other);
    };
}

inline void EnemyScript::onUpdate(Scene &scene, Entity &entity)
{
    Transform &position = scene.getComponent<Transform>(entity);
    Velocity &velocity = scene.getComponent<Velocity>(entity);
    const float amplitude = 3.0f;
    const float sineSpeed = 0.01f;

    position.y += amplitude * std::sin(sineSpeed * position.x);
    velocity.dy = amplitude * sineSpeed * std::cos(sineSpeed * position.x);
}

#endif /* !ENEMYSCRIPT_HPP_ */
