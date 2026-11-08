#ifndef ENEMYSCRIPT_HPP_
#define ENEMYSCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Collider2D.hpp"
#include "PlayerScript.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include "Velocity.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <string>
#include <vector>

inline const std::vector<std::string> EnemyScript_params = {"PowerUpChance", "Amplitude"};

class EnemyScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
    void setParameter(const std::string &name, const std::string &value) override;

    static void kill(Scene &scene, Entity enemy);

  private:
    static constexpr std::uint32_t PLAYER_LAYER = 1;
    static constexpr std::uint32_t DEAD_LAYER = 0;

    static bool isClientSide(Scene &scene);
    static bool isDead(Scene &scene, Entity enemy);
    void explode(Scene &scene, Entity enemy) const;
    static bool rollPowerUp(int chance);
    static void spawnAt(Scene &scene, const std::string &prefab, const Transform &origin);

    int _powerUpChance = 1000;
    float _amplitude = 3.0f;
};

inline void EnemyScript::setParameter(const std::string &name, const std::string &value)
{
    if (name == "PowerUpChance")
        _powerUpChance = std::stoi(value);
    if (name == "Amplitude")
        _amplitude = std::stof(value);
}

inline bool EnemyScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void EnemyScript::kill(Scene &scene, Entity enemy)
{
    scene.getComponent<Collider2D>(enemy).layer = DEAD_LAYER;
}

inline bool EnemyScript::isDead(Scene &scene, Entity enemy)
{
    return scene.getComponent<Collider2D>(enemy).layer == DEAD_LAYER;
}

inline bool EnemyScript::rollPowerUp(int chance)
{
    if (chance <= 0)
        return false;
    static std::mt19937 generator{std::random_device{}()};
    std::uniform_int_distribution<int> distribution(1, chance);

    return distribution(generator) == 1;
}

inline void EnemyScript::spawnAt(Scene &scene, const std::string &prefab, const Transform &origin)
{
    Entity spawned = scene.getResource<PrefabsManager>().InstantiatePrefab(prefab, scene);
    Transform &position = scene.getComponent<Transform>(spawned);

    position.x = origin.x;
    position.y = origin.y;
}

inline void EnemyScript::explode(Scene &scene, Entity enemy) const
{
    const Transform origin = scene.getComponent<Transform>(enemy);

    spawnAt(scene, "Explosion", origin);
    if (rollPowerUp(_powerUpChance))
        spawnAt(scene, "PowerUp", origin);
    scene.queueDestroy(enemy);
}

inline void EnemyScript::onStart(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    scene.getComponent<Collider2D>(entity).onCollision = [&scene](Entity enemy, Entity other) {
        if (!scene.isEntityAlive(other) || isDead(scene, enemy))
            return;
        if (scene.getComponent<Collider2D>(other).layer != PLAYER_LAYER)
            return;
        PlayerScript::kill(scene, other);
    };
}

inline void EnemyScript::onUpdate(Scene &scene, Entity &entity)
{
    if (!isClientSide(scene) && isDead(scene, entity)) {
        explode(scene, entity);
        return;
    }
    Transform &position = scene.getComponent<Transform>(entity);
    Velocity &velocity = scene.getComponent<Velocity>(entity);
    const float sineSpeed = 0.01f;

    position.y += _amplitude * std::sin(sineSpeed * position.x);
    velocity.dy = _amplitude * sineSpeed * std::cos(sineSpeed * position.x);
}

#endif /* !ENEMYSCRIPT_HPP_ */
