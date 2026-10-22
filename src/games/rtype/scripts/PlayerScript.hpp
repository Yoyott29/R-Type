#ifndef PLAYERSCRIPT_HPP_
#define PLAYERSCRIPT_HPP_

#include "Position.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Shape.hpp"
#include "Sprite.hpp"

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
    Sprite &sprite = scene.getComponent<Sprite>(entity);
    float offsetX = std::holds_alternative<BoxShape>(sprite.shape)
                        ? std::get<BoxShape>(sprite.shape).width / 2.0f
                        : std::get<CircleShape>(sprite.shape).radius;
    float offsetY = std::holds_alternative<BoxShape>(sprite.shape)
                        ? std::get<BoxShape>(sprite.shape).height / 2.0f
                        : std::get<CircleShape>(sprite.shape).radius;

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
