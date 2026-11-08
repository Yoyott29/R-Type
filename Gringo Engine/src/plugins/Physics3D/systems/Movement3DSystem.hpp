/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Movement3DSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Velocity3D.hpp"

class Movement3DSystem : public System
{
  public:
    void update(Scene &scene) override;
};
