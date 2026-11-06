/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** IncreaseScore
*/

#pragma once

#include "GameScore.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include "UIText.hpp"
#include "Velocity.hpp"
#include <cmath>

class IncreaseScore : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
    int score = 0;

  protected:
  private:
};

inline void IncreaseScore::onStart(Scene &, Entity &) { score = 0; }

inline void IncreaseScore::onUpdate(Scene &scene, Entity &entity)
{
    if (!scene.hasComponent<UIText>(entity))
        return;
    UIText &text = scene.getComponent<UIText>(entity);
    text.content = "score : " + std::to_string(scene.getResource<GameScore>().value);
}
