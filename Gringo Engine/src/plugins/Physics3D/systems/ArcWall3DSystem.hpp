/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ArcWall3DSystem
*/

#pragma once
#include "ArcWall3D.hpp"
#include "Collider3D.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Transform.hpp"

// Radians to degrees
#define DEGREES (180.0f / 3.14159265358979323846f)

// Pushes the moving bodies (non static RigidBody3D with a Velocity3D and a solid Collider3D)
// back to the allowed side of every ArcWall3D, and stops their speed toward the wall. Runs
// after Collider3DSystem
class ArcWall3DSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    // An angle in degrees brought into [0, 360)
    static float wrap(float degrees);
    static bool inSector(const ArcWall3D &wall, float dx, float dz);
    static void push(Scene &scene, Entity body, const Transform &centre, const ArcWall3D &wall);
};
