#pragma once

#include "KeyboardInput.hpp"
#include "Position.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "keyboard.hpp"

class PlayerScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
};

void PlayerScript::onStart(Scene &, Entity &) {}

void PlayerScript::onUpdate(Scene &scene, Entity &entity)
{
    Position &position = scene.getComponent<Position>(entity);
    const KeyboardInput &keyboard = scene.getResource<KeyboardInput>();
    float speed = 5.0f;

    if (keyboard.isDown(Key::Up))
        position.y -= speed;
    if (keyboard.isDown(Key::Down))
        position.y += speed;
    if (keyboard.isDown(Key::Left))
        position.x -= speed;
    if (keyboard.isDown(Key::Right))
        position.x += speed;
}
