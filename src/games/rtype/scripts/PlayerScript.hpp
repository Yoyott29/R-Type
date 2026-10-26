#ifndef PLAYERSCRIPT_HPP_
#define PLAYERSCRIPT_HPP_

#include "Collider2D.hpp"
#include "Position.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Shape.hpp"

class PlayerScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
};

inline void PlayerScript::onStart(Scene &, Entity &) {}

inline void PlayerScript::onUpdate(Scene &scene, Entity &entity)
{
    Position &position = scene.getComponent<Position>(entity);
    Collider2D &collider = scene.getComponent<Collider2D>(entity);
    float offsetX = std::holds_alternative<BoxShape>(collider.shape)
                        ? std::get<BoxShape>(collider.shape).width / 2.0f
                        : std::get<CircleShape>(collider.shape).radius;
    float offsetY = std::holds_alternative<BoxShape>(collider.shape)
                        ? std::get<BoxShape>(collider.shape).height / 2.0f
                        : std::get<CircleShape>(collider.shape).radius;

    if (position.x - offsetX < 0.0f)
        position.x = offsetX;
    if (position.x + offsetX > 1920.0f)
        position.x = 1920.0f - offsetX;
    if (position.y - offsetY < 0.0f)
        position.y = offsetY;
    if (position.y + offsetY > 1080.0f)
        position.y = 1080.0f - offsetY;
}

#endif /* !PLAYERSCRIPT_HPP_ */
