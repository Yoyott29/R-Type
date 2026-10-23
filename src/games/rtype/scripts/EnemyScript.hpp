#ifndef ENEMYSCRIPT_HPP_
#define ENEMYSCRIPT_HPP_

#include "Position.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Velocity.hpp"
#include <cmath>

class EnemyScript : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
};

inline void EnemyScript::onStart(Scene &, Entity &) {}

inline void EnemyScript::onUpdate(Scene &scene, Entity &entity)
{
    Position &position = scene.getComponent<Position>(entity);
    Velocity &velocity = scene.getComponent<Velocity>(entity);
    constexpr float amplitude = 3.0f;
    constexpr float sineSpeed = 0.01f;

    position.y += amplitude * std::sin(sineSpeed * position.x);
    velocity.dy = amplitude * sineSpeed * std::cos(sineSpeed * position.x);
}

#endif /* !ENEMYSCRIPT_HPP_ */
