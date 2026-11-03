#ifndef PLAYERSCRIPT_HPP_
#define PLAYERSCRIPT_HPP_

#include "Collider2D.hpp"
#include "Position.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Shape.hpp"
#include "Time.hpp"
#ifdef GRINGO_USE_RENDERER
#include "KeyboardInput.hpp"
#endif

inline const std::vector<std::string> PlayerScript_params = {"speed", "health"};

class PlayerScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
    void setParameter(const std::string &name, const std::string &value) override;

  private:
    float _speed;
};

inline void PlayerScript::setParameter(const std::string &name, const std::string &value)
{
    if (name == "speed")
        _speed = std::stof(value);
}

inline void PlayerScript::onStart(Scene &, Entity &) {}

inline void PlayerScript::onUpdate(Scene &scene, Entity &entity)
{
    Position &position = scene.getComponent<Position>(entity);
    Collider2D &collider = scene.getComponent<Collider2D>(entity);
#ifdef GRINGO_USE_RENDERER
    const KeyboardInput &keyboard = scene.getResource<KeyboardInput>();
#endif
    float offsetX = std::holds_alternative<BoxShape>(collider.shape)
                        ? std::get<BoxShape>(collider.shape).width / 2.0f
                        : std::get<CircleShape>(collider.shape).radius;
    float offsetY = std::holds_alternative<BoxShape>(collider.shape)
                        ? std::get<BoxShape>(collider.shape).height / 2.0f
                        : std::get<CircleShape>(collider.shape).radius;

#ifdef GRINGO_USE_RENDERER
    if (keyboard.isDown(Key::Up) || keyboard.isDown(Key::Z))
        position.y -= _speed * FIXED_DELTA_TIME;
    if (keyboard.isDown(Key::Down) || keyboard.isDown(Key::S))
        position.y += _speed * FIXED_DELTA_TIME;
    if (keyboard.isDown(Key::Left) || keyboard.isDown(Key::Q))
        position.x -= _speed * FIXED_DELTA_TIME;
    if (keyboard.isDown(Key::Right) || keyboard.isDown(Key::D))
        position.x += _speed * FIXED_DELTA_TIME;
#endif

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
