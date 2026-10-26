#ifndef DESTROYOFFSCREENSCRIPT_HPP_
#define DESTROYOFFSCREENSCRIPT_HPP_

#include "Position.hpp"
#include "Scene.hpp"
#include "Script.hpp"

class DestroyOffScreenScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
};

inline void DestroyOffScreenScript::onStart(Scene &, Entity &) {}

inline void DestroyOffScreenScript::onUpdate(Scene &scene, Entity &entity)
{
    Position const &position = scene.getComponent<Position>(entity);
    if (position.x < -50.0f || position.x > 2050.0f || position.y < -50.0f || position.y > 1130.0f)
        scene.queueDestroy(entity);
}

#endif /* !DESTROYOFFSCREENSCRIPT_HPP_ */
