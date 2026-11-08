#ifndef ENEMYSHOOTSCRIPT_HPP_
#define ENEMYSHOOTSCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "Difficulty.hpp"
#include "MatchState.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Velocity.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <random>

class EnemyShootScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

    static std::optional<Transform> nearestPlayer(Scene &scene, const Transform &from);

  private:
    static constexpr float FIRST_MIN_COOLDOWN = 0.5f;
    static constexpr float FIRST_MAX_COOLDOWN = 1.5f;
    static constexpr float MIN_COOLDOWN = 2.0f;
    static constexpr float MAX_COOLDOWN = 4.0f;
    static constexpr float BULLET_SPEED = 400.0f;
    static constexpr std::uint32_t PLAYER_LAYER = 1;

    static bool isClientSide(Scene &scene);
    // Random cooldown in [min, max], scaled by the difficulty
    static float randomCooldown(Scene &scene, float min, float max);
    static void shoot(Scene &scene, const Transform &from, const Transform &target);

    float _cooldown = 0.0f;
};

inline bool EnemyShootScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline float EnemyShootScript::randomCooldown(Scene &scene, float min, float max)
{
    static std::mt19937 generator{std::random_device{}()};
    std::uniform_real_distribution<float> distribution(min, max);

    return distribution(generator) * Difficulties::current(scene).fireCooldownScale;
}

inline void EnemyShootScript::onStart(Scene &scene, Entity &)
{
    _cooldown = randomCooldown(scene, FIRST_MIN_COOLDOWN, FIRST_MAX_COOLDOWN);
}

inline void EnemyShootScript::onUpdate(Scene &scene, Entity &entity)
{
    if (isClientSide(scene) || isWaitingForMatch(scene))
        return;
    _cooldown -= FIXED_DELTA_TIME;
    if (_cooldown > 0.0f)
        return;
    _cooldown = randomCooldown(scene, MIN_COOLDOWN, MAX_COOLDOWN);
    const Transform from = scene.getComponent<Transform>(entity);
    const std::optional<Transform> target = nearestPlayer(scene, from);
    if (target)
        shoot(scene, from, *target);
}

inline std::optional<Transform> EnemyShootScript::nearestPlayer(Scene &scene, const Transform &from)
{
    std::optional<Transform> nearest;
    float bestDistance = 0.0f;

    for (Entity player : scene.view<Transform, Collider2D>()) {
        if (scene.getComponent<Collider2D>(player).layer != PLAYER_LAYER)
            continue;
        const Transform &position = scene.getComponent<Transform>(player);
        const float distance = std::hypot(position.x - from.x, position.y - from.y);
        if (!nearest || distance < bestDistance) {
            nearest = position;
            bestDistance = distance;
        }
    }
    return nearest;
}

inline void EnemyShootScript::shoot(Scene &scene, const Transform &from, const Transform &target)
{
    const float dx = target.x - from.x;
    const float dy = target.y - from.y;
    const float length = std::max(1.0f, std::hypot(dx, dy));
    const float speed = BULLET_SPEED * Difficulties::current(scene).bulletSpeedScale;
    Entity bullet = scene.getResource<PrefabsManager>().InstantiatePrefab("EnemyBullet", scene);

    scene.getComponent<Transform>(bullet).x = from.x;
    scene.getComponent<Transform>(bullet).y = from.y;
    scene.getComponent<Velocity>(bullet) = Velocity{dx / length * speed, dy / length * speed};
}

#endif /* !ENEMYSHOOTSCRIPT_HPP_ */
