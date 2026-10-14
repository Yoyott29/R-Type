/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Game
*/

#include "Game.hpp"
#include <chrono>
#include <random>

Game::Game() : _spawnTimer(0.0f) {}

void Game::runGameLoop(Scene &scene, IRenderer &renderer)
{
    if (_spawnTimer >= 1.0f) {
        Entity newEnemy = scene.createEntity();
        float randomY = static_cast<float>(rand() % 1080);
        scene.addComponent(newEnemy, Position{2000.0f, randomY});
        scene.addComponent(newEnemy, Velocity{-150.0f, 0.0f});
        scene.addComponent(newEnemy, Sprite{BoxShape{50.0f, 35.0f}, ColorRGBA{228, 48, 0, 255}});
        scene.addComponent(newEnemy, Collider2D{BoxShape{50.0f, 35.0f}, nullptr});
        _spawnTimer = 0.0f;
    }
    _spawnTimer += renderer.getFrameTime();
}
