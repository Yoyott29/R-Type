/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** MovementSystem
*/

#pragma once
#include "Position.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Time.hpp"
#include "Velocity.hpp"

class MovementSystem : public System
{
  public:
    void update(Scene &scene);
};
