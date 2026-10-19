#ifndef ENEMYSPAWNERSCRIPT_HPP_
#define ENEMYSPAWNERSCRIPT_HPP_

#include "Position.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include <cstdlib>
#include <ctime>
#include <vector>

class EnemySpawnerScript : public GringoEntityScript
{
  public:
    EnemySpawnerScript(PrefabsManager &prefabs, std::vector<Entity> &entities);
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    PrefabsManager &_prefabs;
    std::vector<Entity> &_entities;
    float _spawnTimer = 0.0f;
    float _waveTimer = 0.0f;
    int _enemyCount = 0;
    int _waveType = 1;
    float _waveDelay = 0.5f;
    float _waveY = 0.0f;
};

inline EnemySpawnerScript::EnemySpawnerScript(PrefabsManager &prefabs,
                                              std::vector<Entity> &entities)
    : _prefabs(prefabs), _entities(entities)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    _waveDelay = static_cast<float>(std::rand() % 10) * 0.1f + 0.5f;
}

inline void EnemySpawnerScript::onStart(Scene &, Entity &) {}

inline void EnemySpawnerScript::onUpdate(Scene &scene, Entity &)
{
    if (_enemyCount == 0) {
        _waveTimer += FIXED_DELTA_TIME;
        if (_waveTimer < _waveDelay)
            return;
        _enemyCount = 5 + std::rand() % 6;
        _waveType = 1 + std::rand() % 2;
        _waveY = static_cast<float>(std::rand() % 1080);
        _waveTimer = 0.0f;
        _spawnTimer = 0.3f;
    }
    _spawnTimer += FIXED_DELTA_TIME;
    if (_spawnTimer < 0.2f)
        return;

    Entity enemy = _prefabs.InstantiatePrefab(_waveType == 2 ? "Enemy2" : "Enemy", scene);
    scene.getComponent<Position>(enemy).y = _waveY;
    _spawnTimer = 0.0f;
    --_enemyCount;
    _entities.push_back(enemy);
}

#endif /* !ENEMYSPAWNERSCRIPT_HPP_ */
