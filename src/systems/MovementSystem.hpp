/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** MovementSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"
#include "Time.hpp"

class MovementSystem : public System
{
  public:
    void update(Scene &scene);
};
