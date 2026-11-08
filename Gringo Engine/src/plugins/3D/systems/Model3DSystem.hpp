/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Model3DSystem
*/

#pragma once
#include "ActiveCamera3D.hpp"
#include "IRenderer.hpp"
#include "Model3D.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Transform.hpp"

class Model3DSystem : public System
{
  public:
    void update(Scene &scene) override;
};
