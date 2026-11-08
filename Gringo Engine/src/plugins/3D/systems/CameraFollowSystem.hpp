#pragma once
#include "Camera.hpp"
#include "CameraFollow3D.hpp"
#include "ChildOf.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Transform.hpp"

class CameraFollowSystem : public System
{
  public:
    void update(Scene &scene) override;
};
