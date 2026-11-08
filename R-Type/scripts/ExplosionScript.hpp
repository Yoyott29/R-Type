#ifndef EXPLOSIONSCRIPT_HPP_
#define EXPLOSIONSCRIPT_HPP_

#include "BuildVariantName.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"

class ExplosionScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;

  private:
    static constexpr float LIFETIME = 0.5f;

    static bool isClientSide(Scene &scene);

    float _age = 0.0f;
};

inline bool ExplosionScript::isClientSide(Scene &scene)
{
    return scene.hasResource<BuildVariantName>() &&
           scene.getResource<BuildVariantName>().name == "client";
}

inline void ExplosionScript::onStart(Scene &, Entity &) {}

inline void ExplosionScript::onUpdate(Scene &scene, Entity &entity)
{
    if (isClientSide(scene))
        return;
    _age += FIXED_DELTA_TIME;
    if (_age >= LIFETIME)
        scene.queueDestroy(entity);
}

#endif /* !EXPLOSIONSCRIPT_HPP_ */
