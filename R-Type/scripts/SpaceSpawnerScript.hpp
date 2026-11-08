#ifndef SPACESPAWNERSCRIPT_HPP_
#define SPACESPAWNERSCRIPT_HPP_

#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <cstdlib>

class SpaceSpawnerScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    float _spawnTimer = 0.0f;
};

inline void SpaceSpawnerScript::onStart(Scene &scene, Entity &)
{
    for (int i = 0; i < 20; ++i) {
        Entity space = scene.getResource<PrefabsManager>().InstantiatePrefab("Star", scene);
        scene.getComponent<Transform>(space).x = static_cast<float>(std::rand() % 1920);
        scene.getComponent<Transform>(space).y = static_cast<float>(std::rand() % 1080);
    }
}

inline void SpaceSpawnerScript::onUpdate(Scene &scene, Entity &)
{
    _spawnTimer += FIXED_DELTA_TIME;
    if (_spawnTimer < 0.5f)
        return;

    Entity space = scene.getResource<PrefabsManager>().InstantiatePrefab("Star", scene);
    scene.getComponent<Transform>(space).y = static_cast<float>(std::rand() % 1080);
    _spawnTimer = 0.0f;
}

#endif /* !SPACESPAWNERSCRIPT_HPP_ */
