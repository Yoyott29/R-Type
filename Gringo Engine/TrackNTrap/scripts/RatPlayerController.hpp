/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RatPlayerController
*/

#pragma once

#include "Collider3D.hpp"
#include "GameState.hpp"
#include "IRenderer.hpp"
#include "KeyboardInput.hpp"
#include "RigidBody3D.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include "Velocity3D.hpp"
#include <cmath>

std::vector<std::string> RatPlayerController_params = {"speed", "sensitivity"};

class RatPlayerController : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override
    {
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        spawn = scene.getComponent<Transform>(entity);
        // Only called for triggers: the floor is solid, every trigger kills the rat
        scene.getComponent<Collider3D>(entity).onCollision = [this](Entity, Entity) {
            dead = true;
        };
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (!scene.getResource<GameState>().canMove)
            return;
        Transform &transform = scene.getComponent<Transform>(entity);
        const KeyboardInput &keyboard = scene.getResource<KeyboardInput>();
        RigidBody3D &body = scene.getComponent<RigidBody3D>(entity);
        Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);
        IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();

        if (dead) {
            transform = spawn;
            velocity = Velocity3D{};
            dead = false;
        }

        transform.rotationY += renderer.getMouseDelta().x * sensitivity;

        if (transform.rotationY >= 360.0f)
            transform.rotationY -= 360.0f;
        if (transform.rotationY < 0.0f)
            transform.rotationY += 360.0f;

        const float radians = transform.rotationY * 3.14159265358979323846f / 180.0f;
        const float forwardX = std::sin(radians);
        const float forwardZ = -std::cos(radians);
        const float movementSpeed =
            keyboard.isDown(Key::LeftShift) ? speed * 2.0f : speed;
        const float forward = keyboard.isDown(Key::Z) ? movementSpeed : 0.0f;

        velocity.dx = forward * forwardX;
        velocity.dz = forward * forwardZ;

        const bool jumpDown = keyboard.isDown(Key::Space);
        if (jumpDown && !_jumpWasDown && body.grounded)
            velocity.dy = JUMP_FORCE;
        _jumpWasDown = jumpDown;

        fitCollider(scene.getComponent<Collider3D>(entity), transform, radians);
    }

    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "speed")
            speed = std::stof(value);
        else if (name == "sensitivity")
            sensitivity = std::stof(value);
    }

  private:
    // Colliders do not rotate: the box around the body, turned like the rat
    static void fitCollider(Collider3D &collider, const Transform &transform, float radians)
    {
        const float length = BODY_LENGTH * transform.scaleX;
        const float width = BODY_WIDTH * transform.scaleX;
        const float sine = std::abs(std::sin(radians));
        const float cosine = std::abs(std::cos(radians));

        collider.width = length * sine + width * cosine;
        collider.depth = length * cosine + width * sine;
    }

    // Size of the model's body without the tail, nose in front of the center of the collider
    static constexpr float BODY_LENGTH = 4.5f;
    static constexpr float BODY_WIDTH = 1.1f;

    float speed = 6.0f;
    float sensitivity = 0.2f;
    static constexpr float JUMP_FORCE = 5.0f;
    Transform spawn;
    bool dead = false;
    bool _jumpWasDown = false;
};
