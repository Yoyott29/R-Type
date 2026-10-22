#ifndef SPACESPAWNERSCRIPT_HPP_
#define SPACESPAWNERSCRIPT_HPP_

#include "Position.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include <cstdlib>

class SpaceSpawnerScript : public GringoEntityScript
{
  public:
    SpaceSpawnerScript(PrefabsManager &prefabs);
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    PrefabsManager &_prefabs;
    float _spawnTimer = 0.0f;
};

inline SpaceSpawnerScript::SpaceSpawnerScript(PrefabsManager &prefabs) : _prefabs(prefabs) {}

inline void SpaceSpawnerScript::onStart(Scene &, Entity &) {}

inline void SpaceSpawnerScript::onUpdate(Scene &scene, Entity &)
{
    _spawnTimer += FIXED_DELTA_TIME;
    if (_spawnTimer < 0.5f)
        return;

    Entity space = _prefabs.InstantiatePrefab("Star", scene);
    scene.getComponent<Position>(space).y = static_cast<float>(std::rand() % 1080);
    _spawnTimer = 0.0f;
}

#endif /* !SPACESPAWNERSCRIPT_HPP_ */
