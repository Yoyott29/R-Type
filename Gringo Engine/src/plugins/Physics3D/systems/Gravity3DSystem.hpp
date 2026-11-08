/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Gravity3DSystem
*/

#pragma once
#include "Gravity3D.hpp"
#include "RigidBody3D.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Time.hpp"
#include "Velocity3D.hpp"

class Gravity3DSystem : public System
{
  public:
    void update(Scene &scene) override;
};
