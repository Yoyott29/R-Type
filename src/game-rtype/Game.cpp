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

Entity Game::SpawnEnemy(Scene &scene)
{
    if (_spawnTimer >= 1.0f) {
        Entity enemy = _prefabs.InstantiatePrefab("Enemy", scene);
        InitializeEnemyPosition(scene, enemy);
        _spawnTimer = 0.0f;
        return enemy;
    }
    _spawnTimer += FIXED_DELTA_TIME;
    return static_cast<Entity>(NULL_INDEX);
}

void Game::InitializeEnemyPosition(Scene &scene, Entity enemy)
{
    float randomY = static_cast<float>(rand() % 1080);
    scene.getComponent<Position>(enemy).y = randomY;
}

void Game::killOffScreenEnemy(Scene &scene, Entity enemy)
{
    Position &pos = scene.getComponent<Position>(enemy);

    if (pos.x < -50.0f || pos.y < -50.0f || pos.y > 1130.0f)
        scene.queueDestroy(enemy);
}

void Game::maintainPlayerOnScreen(Scene &scene, Entity player)
{
    Position &pos = scene.getComponent<Position>(player);
    Sprite &sprite = scene.getComponent<Sprite>(player);

    float offsetX = std::holds_alternative<BoxShape>(sprite.shape)
                        ? std::get<BoxShape>(sprite.shape).width / 2.0f
                        : std::get<CircleShape>(sprite.shape).radius;
    float offsetY = std::holds_alternative<BoxShape>(sprite.shape)
                        ? std::get<BoxShape>(sprite.shape).height / 2.0f
                        : std::get<CircleShape>(sprite.shape).radius;

    if (pos.x - offsetX < 0.0f)
        pos.x = 0.0f + offsetX;
    if (pos.x + offsetX > 1920.0f)
        pos.x = 1920.0f - offsetX;
    if (pos.y - offsetY < 0.0f)
        pos.y = 0.0f + offsetY;
    if (pos.y + offsetY > 1080.0f)
        pos.y = 1080.0f - offsetY;
}
