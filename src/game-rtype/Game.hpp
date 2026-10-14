/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Game
*/

#ifndef GAME_HPP_
#define GAME_HPP_
#include "Collider2D.hpp"
#include "Entity.hpp"
#include "IRenderer.hpp"
#include "Position.hpp"
#include "PrefabsManager.hpp"
#include "RaylibRenderer.hpp"
#include "Scene.hpp"
#include "Sprite.hpp"
#include "Velocity.hpp"

class Game
{
  public:
    Game(PrefabsManager &prefabs);
    ~Game() = default;

    void SpawnEnemy(Scene &scene);
    void InitializeEnemyPosition(Scene &scene, Entity enemy);

  protected:
  private:
    float _spawnTimer;
    PrefabsManager &_prefabs;
};

#endif /* !GAME_HPP_ */
