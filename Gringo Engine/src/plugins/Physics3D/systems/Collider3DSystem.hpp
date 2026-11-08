/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Collider3DSystem
*/

#pragma once
#include "Collider3D.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "Transform.hpp"
#include "Velocity3D.hpp"
#include <set>

class Collider3DSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    struct Vector {
        float x;
        float y;
        float z;
    };

    // A collider in the world: its middle, its three directions, half its size along each of
    // them, and half the size of the box lined up with the world around it
    struct Box {
        Vector center;
        Vector axisX;
        Vector axisY;
        Vector axisZ;
        float halfWidth;
        float halfHeight;
        float halfDepth;
        Vector extent;
    };

    static Box box(const Transform &position, const Collider3D &collider);
    static Vector turn(Vector vector, const Transform &position, const Collider3D &collider);
    static float dot(Vector first, Vector second);
    static Vector cross(Vector first, Vector second);
    // True if a flat wall going along this direction can be put between the two boxes
    static bool apart(const Box &first, const Box &second, Vector axis);
    // True if the two boxes don't touch
    static bool separated(const Box &first, const Box &second);
    static bool dynamic(Scene &scene, Entity entity);
    // True if `body` moves and the top of `step` is above its feet by its RigidBody3D stepHeight
    // at most
    static bool climbs(Scene &scene, Entity body, const Box &bodyBox, const Box &stepBox);
    static void resolve(Scene &scene, Entity first, Entity second, Transform &firstPosition,
                        Transform &secondPosition, float overlapX, float overlapY, float overlapZ,
                        Vector delta);
    std::set<std::pair<Entity, Entity>> _solidContacts;
};
