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
#include "RaylibRenderer.hpp"
#include "Scene.hpp"
#include "Sprite.hpp"
#include "Velocity.hpp"

class Game
{
  public:
    Game();
    ~Game() = default;

    void runGameLoop(Scene &scene, IRenderer &renderer);

  protected:
  private:
    float _spawnTimer;
};

#endif /* !GAME_HPP_ */
