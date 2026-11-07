/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Collider3DSystem
*/

#pragma once
#include "Collider3D.hpp"
#include "Transform.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Velocity3D.hpp"
#include <set>

class Collider3DSystem : public System {
  public:
    void update(Scene &scene) override;

  private:
    static bool dynamic(Scene &scene, Entity entity);
    // True if `body` moves and the top of `step` is above its feet by its RigidBody3D stepHeight
    // at most
    static bool climbs(Scene &scene, Entity body, const Transform &bodyPosition,
                       const Collider3D &bodyCollider, const Transform &stepPosition,
                       const Collider3D &stepCollider);
    static void resolve(Scene &scene, Entity first, Entity second, Transform &firstPosition,
                        Transform &secondPosition, const Collider3D &firstCollider,
                        const Collider3D &secondCollider, const float overlapX,
                        const float overlapY, const float overlapZ);
    std::set<std::pair<Entity, Entity>> _solidContacts;
};
