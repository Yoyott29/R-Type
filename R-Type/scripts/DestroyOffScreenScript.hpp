#ifndef DESTROYOFFSCREENSCRIPT_HPP_
#define DESTROYOFFSCREENSCRIPT_HPP_

#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"

class DestroyOffScreenScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
};

inline void DestroyOffScreenScript::onStart(Scene &, Entity &) {}

inline void DestroyOffScreenScript::onUpdate(Scene &scene, Entity &entity)
{
    Transform const &position = scene.getComponent<Transform>(entity);
    if (position.x < 0.0f || position.x > 2050.0f || position.y < -50.0f || position.y > 1130.0f)
        scene.queueDestroy(entity);
}

#endif /* !DESTROYOFFSCREENSCRIPT_HPP_ */
