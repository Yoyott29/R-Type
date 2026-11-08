#ifndef POWARMORSPAWNERSCRIPT_HPP_
#define POWARMORSPAWNERSCRIPT_HPP_

#include "BossState.hpp"
#include "BuildVariantName.hpp"
#include "GameScore.hpp"
#include "MatchState.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include <cstdint>
#include <random>

class PowArmorSpawnerScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static constexpr std::int32_t SCORE_STEP = 3000;
    static constexpr float MIN_Y = 250.0f;
    static constexpr float MAX_Y = 830.0f;

    static bool isClientSide(Scene &scene);
    static float randomY();
    static void spawn(Scene &scene);

    std::int32_t _nextScore = SCORE_STEP;
};

inline bool PowArmorSpawnerScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline float PowArmorSpawnerScript::randomY()
{
    static std::mt19937 generator{std::random_device{}()};
    std::uniform_real_distribution<float> distribution(MIN_Y, MAX_Y);

    return distribution(generator);
}

inline void PowArmorSpawnerScript::spawn(Scene &scene)
{
    Entity armor = scene.getResource<PrefabsManager>().InstantiatePrefab("PowArmor", scene);

    scene.getComponent<Transform>(armor).y = randomY();
}

inline void PowArmorSpawnerScript::onStart(Scene &, Entity &) {}

inline void PowArmorSpawnerScript::onUpdate(Scene &scene, Entity &)
{
    if (isClientSide(scene))
        return;
    if (isWaitingForMatch(scene)) {
        _nextScore = SCORE_STEP;
        return;
    }
    if (isBossActive(scene) || scene.getResource<GameScore>().value < _nextScore)
        return;
    _nextScore += SCORE_STEP;
    spawn(scene);
}

#endif /* !POWARMORSPAWNERSCRIPT_HPP_ */
