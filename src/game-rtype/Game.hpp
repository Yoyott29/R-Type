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
#include "Position.hpp"
#include "PrefabsManager.hpp"
#include "Scene.hpp"
#include "Sprite.hpp"
#include "Velocity.hpp"

class Game
{
  public:
    Game(PrefabsManager &prefabs);
    ~Game() = default;

    Entity SpawnEnemy(Scene &scene);
    Entity SpawnSpace(Scene &scene);
    void InitializeEnemyPosition(Scene &scene, Entity enemy);
    void DestroyOffScreenEntity(Scene &scene, Entity enemy);
    void maintainPlayerOnScreen(Scene &scene, Entity player);
    void makeSinusoidalMovement(Scene &scene, Entity enemy);

  protected:
  private:
    float _spawnTimer;
    float _spaceSpawnTimer;
    float _waveTimer;
    int _enemyCount;
    float _waveDelay;
    float _waveY;
    PrefabsManager &_prefabs;
};

#endif /* !GAME_HPP_ */
