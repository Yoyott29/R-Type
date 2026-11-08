#ifndef TRAPPLACER_HPP_
#define TRAPPLACER_HPP_

#include "Collider3D.hpp"
#include "GameState.hpp"
#include "Net.hpp"
#include "NetComponents.hpp"
#include "PrefabsManager.hpp"
#include "RigidBody3D.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include "Traps.hpp"
#include "Velocity3D.hpp"
#include <cmath>
#include <string>
#include <vector>

#define TRAP_DEGREES_TO_RADIANS (3.14159265f / 180.0f)

inline const std::vector<std::string> TrapPlacer_params = {"prefab", "count",    "distance",
                                                           "speed",  "lifetime", "immunity"};

class TrapPlacer : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "prefab")
            _prefab = value;
        else if (name == "count")
            _left = std::stoi(value);
        else if (name == "distance")
            _distance = std::stof(value);
        else if (name == "speed")
            _speed = std::stof(value);
        else if (name == "lifetime")
            _lifetime = std::stof(value);
        else if (name == "immunity")
            _immunity = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        if (Net::isClient(scene))
            return;
        if (!scene.hasResource<Traps>())
            scene.addResource<Traps>();
        if (!scene.hasComponent<RatStatus>(entity))
            scene.addComponent(entity, RatStatus{});
        scene.getComponent<RatStatus>(entity).trapsLeft = _left;
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (Net::isClient(scene) || !scene.hasResource<GameState>())
            return;
        Transform &rat = scene.getComponent<Transform>(entity);
        Collider3D &collider = scene.getComponent<Collider3D>(entity);

        // Where the feet are when on the floor: a trap sent mid-jump still goes along the floor
        if (scene.getComponent<RigidBody3D>(entity).grounded)
            _floorY = rat.y + collider.offsetY - collider.height / 2.0f;

        ActionInput input;
        if (scene.hasComponent<RemoteInput>(entity))
            input = scene.getComponent<RemoteInput>(entity).input;
        bool pressed = input.isActive(ACTION_TRAP);
        if (pressed && !_wasPressed && _left > 0 && scene.getResource<GameState>().canMove)
            send(scene, entity, rat);
        _wasPressed = pressed;
    }

  private:
    void send(Scene &scene, Entity rat, Transform &ratTransform)
    {
        if (!scene.getResource<PrefabsManager>().hasPrefab(_prefab))
            return;
        float radians = ratTransform.rotationY * TRAP_DEGREES_TO_RADIANS;
        Entity trap = scene.getResource<PrefabsManager>().InstantiatePrefab(_prefab, scene);

        if (!scene.hasComponent<Transform>(trap))
            scene.addComponent(trap, Transform{});
        Transform &transform = scene.getComponent<Transform>(trap);
        // The rat faces (sin, -cos) of its rotation: the trap flies that way, its long side first
        float forwardX = std::sin(radians);
        float forwardZ = -std::cos(radians);

        transform.x += ratTransform.x + forwardX * _distance;
        transform.y += _floorY;
        transform.z += ratTransform.z + forwardZ * _distance;
        transform.rotationY += ratTransform.rotationY - 90.0f;
        Velocity3D velocity;
        velocity.dx = forwardX * _speed;
        velocity.dz = forwardZ * _speed;
        if (scene.hasComponent<Velocity3D>(trap))
            scene.getComponent<Velocity3D>(trap) = velocity;
        else
            scene.addComponent(trap, velocity);
        scene.getResource<Traps>().add(trap, rat, _immunity, _lifetime);
        _left -= 1;
        scene.getComponent<RatStatus>(rat).trapsLeft = _left;
    }

    std::string _prefab = "MouseTrap";
    int _left = 3;
    float _distance = 12.0f;
    float _speed = 150.0f;
    float _lifetime = 6.0f;
    float _immunity = 5.0f;
    bool _wasPressed = false;
    float _floorY = 0.0f;
};

#endif /* !TRAPPLACER_HPP_ */
