/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** CameraSystem
*/

#pragma once
#include "ActiveCamera3D.hpp"
#include "Camera.hpp"
#include "Transform.hpp"
#include "Scene.hpp"
#include "System.hpp"

class CameraSystem : public System {
  public:
    void update(Scene &scene) override;
};
