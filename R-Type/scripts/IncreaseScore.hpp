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
#include "UIText.hpp"

class IncreaseScore : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override;
    void onUpdate(Scene &scene, Entity &entity) override;
};

inline void IncreaseScore::onStart(Scene &, Entity &) {}

inline void IncreaseScore::onUpdate(Scene &scene, Entity &entity)
{
    if (!scene.hasComponent<UIText>(entity))
        return;
    UIText &text = scene.getComponent<UIText>(entity);
    text.content = "score : " + std::to_string(scene.getResource<GameScore>().value);
}
