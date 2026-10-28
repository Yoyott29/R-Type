/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** MovementSystem
*/

#include "MovementSystem.hpp"
#include <algorithm>
#include <vector>

void MovementSystem::update(Scene &scene)
{
    ThreadPool &threadPool = scene.getThreadPool();
    std::vector<Entity> entities(_entities.begin(), _entities.end());
    std::size_t chunkSize = entities.size() / threadPool.size() + 1;

    for (std::size_t start = 0; start < entities.size(); start += chunkSize) {
        std::size_t end = std::min(start + chunkSize, entities.size());

        threadPool.addTask([&scene, &entities, start, end] {
            for (std::size_t i = start; i < end; ++i) {
                Position &position = scene.getComponent<Position>(entities[i]);
                Velocity const &velocity = scene.getComponent<Velocity>(entities[i]);

                position.x += velocity.dx * FIXED_DELTA_TIME;
                position.y += velocity.dy * FIXED_DELTA_TIME;
            }
        });
    }
    threadPool.wait();
}
