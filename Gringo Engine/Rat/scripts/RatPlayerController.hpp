/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RatPlayerController
*/

#pragma once

#include "IRenderer.hpp"
#include "KeyboardInput.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include "Velocity3D.hpp"
#include <cmath>

class RatPlayerController : public GringoEntityScript
{
  public:
    void onStart(Scene &, Entity &) override {}

    void onUpdate(Scene &scene, Entity &entity) override
    {
        Transform &transform = scene.getComponent<Transform>(entity);
        const KeyboardInput &keyboard = scene.getResource<KeyboardInput>();
        Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);
        IRenderer &renderer =
            scene.getResource<std::reference_wrapper<IRenderer>>().get();

        constexpr float speed = 5.0f;
        constexpr float sensitivity = 0.2f;
        transform.rotationY += renderer.getMouseDelta().x * sensitivity;

        if (transform.rotationY >= 360.0f)
            transform.rotationY -= 360.0f;
        if (transform.rotationY < 0.0f)
            transform.rotationY += 360.0f;

        const float radians = transform.rotationY * 3.14159265358979323846f / 180.0f;
        const float forwardX = std::sin(radians);
        const float forwardZ = -std::cos(radians);
        const float rightX = std::cos(radians);
        const float rightZ = std::sin(radians);
        float forward = 0.0f;
        float lateral = 0.0f;

        if (keyboard.isDown(Key::Z))
            forward += speed;
        if (keyboard.isDown(Key::S))
            forward -= speed;
        if (keyboard.isDown(Key::D))
            lateral += speed;
        if (keyboard.isDown(Key::Q))
            lateral -= speed;

        velocity.dx = forward * forwardX + lateral * rightX;
        velocity.dz = forward * forwardZ + lateral * rightZ;
    }

    void setParameter(const std::string &, const std::string &) override {}
};
