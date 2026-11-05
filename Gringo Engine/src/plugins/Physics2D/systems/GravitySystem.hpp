/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** GravitySystem
*/

#pragma once
#include "Gravity2D.hpp"
#include "RigidBody2D.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Time.hpp"
#include "Velocity.hpp"

// Accelerates every non static RigidBody2D by Gravity2D * gravityScale. Runs before the
// MovementSystem, so the new velocity is used on the same step
class GravitySystem : public System
{
  public:
    void update(Scene &scene);
};
