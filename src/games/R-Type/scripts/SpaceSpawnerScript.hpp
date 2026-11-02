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
    SpaceSpawnerScript();
    explicit SpaceSpawnerScript(PrefabsManager &) : SpaceSpawnerScript() {}
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    float _spawnTimer = 0.0f;
};

inline SpaceSpawnerScript::SpaceSpawnerScript() {}

inline void SpaceSpawnerScript::onStart(Scene &scene, Entity &)
{
    for (int i = 0; i < 20; ++i) {
        Entity space = scene.getResource<PrefabsManager>().InstantiatePrefab("Star", scene);
        scene.getComponent<Position>(space).x = static_cast<float>(std::rand() % 1920);
        scene.getComponent<Position>(space).y = static_cast<float>(std::rand() % 1080);
    }
}

inline void SpaceSpawnerScript::onUpdate(Scene &scene, Entity &)
{
    _spawnTimer += FIXED_DELTA_TIME;
    if (_spawnTimer < 0.5f)
        return;

    Entity space = scene.getResource<PrefabsManager>().InstantiatePrefab("Star", scene);
    scene.getComponent<Position>(space).y = static_cast<float>(std::rand() % 1080);
    _spawnTimer = 0.0f;
}

#endif /* !SPACESPAWNERSCRIPT_HPP_ */
