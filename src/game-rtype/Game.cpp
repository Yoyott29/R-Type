/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Game
*/

#include "Game.hpp"
#include "Time.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>

Game::Game(PrefabsManager &prefabs)
    : _spawnTimer(0.0f), _spaceSpawnTimer(0.0f), _waveTimer(0.0f), _enemyCount(0),
      _waveType(1), _waveDelay(0.0f), _waveY(0.0f), _prefabs(prefabs)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    _waveDelay = static_cast<float>(rand() % 10) * 0.1f + 0.5f;
}

Entity Game::SpawnSpace(Scene &scene)
{
    if (_spaceSpawnTimer < 0.5f) {
        _spaceSpawnTimer += FIXED_DELTA_TIME;
        return static_cast<Entity>(NULL_INDEX);
    }

    Entity space = _prefabs.InstantiatePrefab("Star", scene);
    scene.getComponent<Position>(space).y = static_cast<float>(rand() % 1080);
    _spaceSpawnTimer = 0.0f;
    return space;
}

Entity Game::SpawnEnemy(Scene &scene)
{
    if (_enemyCount == 0) {
        _waveTimer += FIXED_DELTA_TIME;
        if (_waveTimer < _waveDelay)
            return static_cast<Entity>(NULL_INDEX);

        _enemyCount = 5 + rand() % 6;
        _waveType = 1 + rand() % 2;
        _waveY = static_cast<float>(rand() % 1080);
        _waveTimer = 0.0f;
        _spawnTimer = 0.3f;
    }

    _spawnTimer += FIXED_DELTA_TIME;
    if (_spawnTimer >= 0.2f) {
        Entity enemy =
            _prefabs.InstantiatePrefab(_waveType == 2 ? "Enemy2" : "Enemy", scene);
        _spawnTimer = 0.0f;
        _enemyCount--;
        scene.getComponent<Position>(enemy).y = _waveY;
        return enemy;
    }
    return static_cast<Entity>(NULL_INDEX);
}

void Game::InitializeEnemyPosition(Scene &scene, Entity enemy)
{
    float randomY = static_cast<float>(rand() % 1080);
    scene.getComponent<Position>(enemy).y = randomY;
}

void Game::DestroyOffScreenEntity(Scene &scene, Entity enemy)
{
    Position &pos = scene.getComponent<Position>(enemy);

    if (pos.x < -50.0f || pos.x > 2050.0f || pos.y < -50.0f || pos.y > 1130.0f)
        scene.queueDestroy(enemy);
}

void Game::makeSinusoidalMovement(Scene &scene, Entity enemy)
{
    Position &position = scene.getComponent<Position>(enemy);
    Velocity &velocity = scene.getComponent<Velocity>(enemy);

    float amplitude = 3.0f;  // Amplitude of the sine wave
    float sineSpeed = 0.01f; // Speed of the sine wave

    position.y += amplitude * std::sin(sineSpeed * position.x);
    velocity.dy = amplitude * sineSpeed * std::cos(sineSpeed * position.x);
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
