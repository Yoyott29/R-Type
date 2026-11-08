#ifndef ENEMYSPAWNERSCRIPT_HPP_
#define ENEMYSPAWNERSCRIPT_HPP_

#include "BossState.hpp"
#include "Difficulty.hpp"
#include "MatchState.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>

class EnemySpawnerScript : public GringoEntityScript
{
  public:
    EnemySpawnerScript();
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static constexpr int MIN_WAVE_SIZE = 1;
    static constexpr int BASE_WAVE_SIZE = 5;
    static constexpr int WAVE_SIZE_SPREAD = 6;

    float _spawnTimer = 0.0f;
    float _waveTimer = 0.0f;
    int _enemyCount = 0;
    int _waveType = 1;
    float _waveDelay = 0.5f;
    float _waveY = 0.0f;
};

inline EnemySpawnerScript::EnemySpawnerScript()
{
    std::srand(
        static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    _waveDelay = static_cast<float>(std::rand() % 10) * 0.1f + 0.5f;
}

inline void EnemySpawnerScript::onStart(Scene &, Entity &) {}

inline void EnemySpawnerScript::onUpdate(Scene &scene, Entity &)
{
    if (isWaitingForMatch(scene) || isBossActive(scene))
        return;
    if (_enemyCount == 0) {
        const DifficultySettings &difficulty = Difficulties::current(scene);
        _waveTimer += FIXED_DELTA_TIME;
        if (_waveTimer < _waveDelay * difficulty.waveDelayScale)
            return;
        const float scaledCount =
            static_cast<float>(BASE_WAVE_SIZE + std::rand() % WAVE_SIZE_SPREAD) *
            difficulty.waveSizeScale;
        _enemyCount = std::max(MIN_WAVE_SIZE, static_cast<int>(std::lround(scaledCount)));
        _waveType = 1 + std::rand() % 2;
        _waveY = static_cast<float>(std::rand() % 1080);
        _waveTimer = 0.0f;
        _spawnTimer = 0.3f;
    }
    _spawnTimer += FIXED_DELTA_TIME;
    if (_spawnTimer < 0.2f)
        return;

    Entity enemy = scene.getResource<PrefabsManager>().InstantiatePrefab(
        _waveType == 2 ? "Enemy2" : "Enemy", scene);
    scene.getComponent<Transform>(enemy).y = _waveY;
    _spawnTimer = 0.0f;
    --_enemyCount;
}

#endif /* !ENEMYSPAWNERSCRIPT_HPP_ */
