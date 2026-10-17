/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Game
*/

#include "Game.hpp"
#include "Time.hpp"
#include <chrono>
#include <random>

Game::Game(PrefabsManager &prefabs) : _spawnTimer(0.0f), _prefabs(prefabs) {}

void Game::SpawnEnemy(Scene &scene)
{
    if (_spawnTimer >= 1.0f) {
        _prefabs.InstantiatePrefab("Enemy", scene);
        _spawnTimer = 0.0f;
    }
    _spawnTimer += FIXED_DELTA_TIME;
}

void Game::InitializeEnemyPosition(Scene &scene, Entity enemy)
{
    float randomY = static_cast<float>(rand() % 1080);
    scene.getComponent<Position>(enemy).y = randomY;
}
