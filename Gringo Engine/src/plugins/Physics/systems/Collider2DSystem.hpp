/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Collider2DSystem
*/

#pragma once
#include "Collider2D.hpp"
#include "Position.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include <algorithm>

struct Point {
    float x;
    float y;
};

class Collider2DSystem : public System
{
  public:
    void update(Scene &scene);
    bool checkCollisions(const Position &positionA, const Collider2D &colliderA,
                         const Position &positionB, const Collider2D &colliderB);

    // Box vs Box
    bool checkOverlaps(const Position &positionA, const BoxShape &colliderA,
                       const Position &positionB, const BoxShape &colliderB);

    // Circle vs Circle
    bool checkOverlaps(const Position &positionA, const CircleShape &colliderA,
                       const Position &positionB, const CircleShape &colliderB);
    // Box vs Circle
    bool checkOverlaps(const Position &positionA, const BoxShape &colliderA,
                       const Position &positionB, const CircleShape &colliderB);
    // Circle vs Box
    bool checkOverlaps(const Position &positionA, const CircleShape &colliderA,
                       const Position &positionB, const BoxShape &colliderB);
};
