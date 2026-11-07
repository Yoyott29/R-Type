/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RatPlayerController
*/

#pragma once

#include "AudioSource.hpp"
#include "ChildOf.hpp"
#include "Collider3D.hpp"
#include "GameState.hpp"
#include "KeyboardInput.hpp"
#include "Model3D.hpp"
#include "RigidBody3D.hpp"
#include "Scene.hpp"
#include "ParticleEmitter3D.hpp"
#include "ProjectComponentManager.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include "Velocity3D.hpp"
#include <algorithm>
#include <cmath>
#include <optional>

// Size of the model's body without the tail, nose in front of the center of the collider
#define BODY_LENGTH 4.5f
#define BODY_WIDTH 1.1f

// Shadow size and forward shift in model units (scaled like the rat), lifted above the
// floor to avoid z-fighting
#define SHADOW_LENGTH 4.2f
#define SHADOW_WIDTH 1.7f
#define SHADOW_FORWARD 0.8f
#define SHADOW_THICKNESS 0.02f
// The floor boxes are up to ~0.27 under the visible track: lifted more than that to stay visible
#define SHADOW_LIFT 0.3f
#define SHADOW_FADE_HEIGHT 15.0f
#define SHADOW_COLOR ColorRGBA{0, 0, 0, 70}

// Point of the run animation (0 to 1) shown while the rat stands still: all four feet on the
// ground, under the body
#define STANDING_FRAME 0.58f

// Dust under the back feet: the ParticleEmitter3D of two children of the rat (the first one on
// the left foot, the second on the right one), started while it runs on the ground, with their
// density times this while sprinting. The feet in model units (scaled like the rat): behind the
// centre, to each side, and above the bottom of the collider
#define DUST_SPRINT_DENSITY 2.0f
#define DUST_FEET_BEHIND 0.8f
#define DUST_FEET_SIDE 0.35f
#define DUST_FEET_HEIGHT 0.3f

// "hit_sound" and "running_sound": editor ids of the entities with the AudioSource played when
// the rat hits an obstacle, and looped while it runs. "camera": editor id of the camera ZQSD
// moves relative to (Z away from it, S toward it)
std::vector<std::string> RatPlayerController_params = {"speed", "jump_force", "hit_sound",
                                                       "running_sound", "camera"};

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
        createShadow(scene, entity);
        setIdle(scene.getComponent<Model3D>(entity), true);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (!scene.getResource<GameState>().canMove)
            return;
        Transform &transform = scene.getComponent<Transform>(entity);
        const KeyboardInput &keyboard = scene.getResource<KeyboardInput>();
        RigidBody3D &body = scene.getComponent<RigidBody3D>(entity);
        Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);

        if (dead) {
            if (const std::optional<Entity> hit = findSound(scene, _hitSoundId))
                scene.getComponent<AudioSource>(*hit).play();
            transform = spawn;
            velocity = Velocity3D{};
            dead = false;
        }

        // The way the camera looks, flat on the floor
        float viewX = 0.0f;
        float viewZ = 0.0f;
        cameraView(scene, transform, viewX, viewZ);
        float moveX = 0.0f;
        float moveZ = 0.0f;
        if (keyboard.isDown(Key::Z)) {
            moveX += viewX;
            moveZ += viewZ;
        }
        if (keyboard.isDown(Key::S)) {
            moveX -= viewX;
            moveZ -= viewZ;
        }
        const float length = std::hypot(moveX, moveZ);
        const bool moving = length > 0.0f;
        const float movementSpeed = keyboard.isDown(Key::LeftShift) ? speed * 2.0f : speed;
        velocity.dx = moving ? moveX / length * movementSpeed : 0.0f;
        velocity.dz = moving ? moveZ / length * movementSpeed : 0.0f;
        // Faces where it runs: forward is (sin, -cos) of its rotation
        if (moving)
            transform.rotationY = std::atan2(moveX, -moveZ) * 180.0f / 3.14159265358979323846f;
        const float radians = transform.rotationY * 3.14159265358979323846f / 180.0f;

        const bool jumpDown = keyboard.isDown(Key::Space);
        if (jumpDown && !_jumpWasDown && body.grounded)
            velocity.dy = jumpForce;
        _jumpWasDown = jumpDown;

        Collider3D &collider = scene.getComponent<Collider3D>(entity);
        fitCollider(collider, transform, radians);
        updateShadow(scene, transform, collider, body.grounded);
        setIdle(scene.getComponent<Model3D>(entity), !moving);
        const bool running = moving && body.grounded;
        updateDust(scene, entity, collider, running, keyboard.isDown(Key::LeftShift));
        updateRunningSound(scene, running);
    }

    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "speed")
            speed = std::stof(value);
        else if (name == "jump_force")
            jumpForce = std::stof(value);
        else if (name == "hit_sound")
            _hitSoundId = std::stoi(value);
        else if (name == "running_sound")
            _runningSoundId = std::stoi(value);
        else if (name == "camera")
            _cameraId = std::stoi(value);
    }

  private:
    // The camera's way of looking at the rat, flat and of length 1. Without a camera: the way the
    // rat faces
    void cameraView(Scene &scene, const Transform &transform, float &viewX, float &viewZ) const
    {
        const float radians = transform.rotationY * 3.14159265358979323846f / 180.0f;
        viewX = std::sin(radians);
        viewZ = -std::cos(radians);
        const std::optional<Entity> camera = scene.getResource<EditorEntityIds>().find(_cameraId);
        if (!camera || !scene.hasComponent<Transform>(*camera))
            return;
        const Transform &eye = scene.getComponent<Transform>(*camera);
        const float distance = std::hypot(transform.x - eye.x, transform.z - eye.z);
        if (distance == 0.0f)
            return;
        viewX = (transform.x - eye.x) / distance;
        viewZ = (transform.z - eye.z) / distance;
    }

    // The entity of an editor id, if it exists and has an AudioSource
    static std::optional<Entity> findSound(Scene &scene, int editorId)
    {
        const std::optional<Entity> sound = scene.getResource<EditorEntityIds>().find(editorId);
        if (!sound || !scene.hasComponent<AudioSource>(*sound))
            return std::nullopt;
        return sound;
    }

    void updateRunningSound(Scene &scene, bool running)
    {
        const std::optional<Entity> entity = findSound(scene, _runningSoundId);
        if (!entity)
            return;
        AudioSource &sound = scene.getComponent<AudioSource>(*entity);
        if (running && !sound.isPlaying())
            sound.play();
        else if (!running && sound.isPlaying())
            sound.stop();
    }

    // Standing still: the run animation held on a frame with the feet on the ground
    static void setIdle(Model3D &model, bool idle)
    {
        model.animationFrame = idle ? STANDING_FRAME : -1.0f;
    }

    // Flat dark ellipse on the floor, under the rat: shows where it will land while jumping
    void createShadow(Scene &scene, Entity entity)
    {
        // Copies: adding components can move the component storage
        const Transform transform = scene.getComponent<Transform>(entity);
        const Collider3D &collider = scene.getComponent<Collider3D>(entity);
        _floorY = transform.y + collider.offsetY - collider.height / 2.0f;
        _shadow = scene.createEntity();
        scene.addComponent<Transform>(_shadow, transform);
        Model3D model;
        model.primitive = "Sphere";
        model.color = SHADOW_COLOR;
        scene.addComponent<Model3D>(_shadow, model);
        updateShadow(scene, transform, scene.getComponent<Collider3D>(entity), true);
    }

    void updateShadow(Scene &scene, const Transform &transform, const Collider3D &collider,
                      bool grounded)
    {
        const float feetY = transform.y + collider.offsetY - collider.height / 2.0f;
        // Only grounded tells where the floor is: keep the last one while in the air
        if (grounded)
            _floorY = feetY;
        // Smaller the higher the rat is
        const float shrink = std::clamp(1.0f - (feetY - _floorY) / SHADOW_FADE_HEIGHT, 0.3f, 1.0f);
        const float radians = transform.rotationY * 3.14159265358979323846f / 180.0f;
        const float forward = SHADOW_FORWARD * transform.scaleX;
        Transform &shadow = scene.getComponent<Transform>(_shadow);
        shadow.x = transform.x + std::sin(radians) * forward;
        shadow.y = _floorY + SHADOW_LIFT;
        shadow.z = transform.z - std::cos(radians) * forward;
        shadow.rotationY = transform.rotationY;
        shadow.scaleX = SHADOW_WIDTH * transform.scaleX * shrink;
        shadow.scaleY = SHADOW_THICKNESS;
        shadow.scaleZ = SHADOW_LENGTH * transform.scaleX * shrink;
    }

    // The children of the rat with a ParticleEmitter3D, looked for until found (the children
    // know their parent once the scene is loaded)
    void findDust(Scene &scene, Entity entity)
    {
        for (Entity child : scene.view<ChildOf, ParticleEmitter3D>()) {
            if (scene.getComponent<ChildOf>(child).parent != entity)
                continue;
            _dust.push_back(child);
            _dustDensity.push_back(scene.getComponent<ParticleEmitter3D>(child).density);
        }
    }

    void updateDust(Scene &scene, Entity entity, const Collider3D &collider, bool running,
                    bool sprinting)
    {
        if (_dust.empty())
            findDust(scene, entity);
        const Transform &transform = scene.getComponent<Transform>(entity);
        const float radians = transform.rotationY * 3.14159265358979323846f / 180.0f;
        const float forwardX = std::sin(radians);
        const float forwardZ = -std::cos(radians);
        const float behind = DUST_FEET_BEHIND * transform.scaleX;
        const float side = DUST_FEET_SIDE * transform.scaleX;
        for (std::size_t i = 0; i < _dust.size(); ++i) {
            // Turned with the rat: ChildOf only follows the parent's position
            const float foot = i == 0 ? -side : side;
            Transform &position = scene.getComponent<Transform>(_dust[i]);
            position.x = transform.x - forwardX * behind - forwardZ * foot;
            position.y = transform.y + collider.offsetY - collider.height / 2.0f +
                         DUST_FEET_HEIGHT * transform.scaleY;
            position.z = transform.z - forwardZ * behind + forwardX * foot;
            // Already where it belongs: the hierarchy must not move it again with the rat
            scene.getComponent<ChildOf>(_dust[i]).lastParentPosition = transform;

            ParticleEmitter3D &dust = scene.getComponent<ParticleEmitter3D>(_dust[i]);
            dust.density = sprinting ? _dustDensity[i] * DUST_SPRINT_DENSITY : _dustDensity[i];
            if (running && !dust.isEmitting())
                dust.start();
            else if (!running && dust.isEmitting())
                dust.stop();
        }
    }

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

    float speed = 6.0f;
    float jumpForce = 5.0f;
    Transform spawn;
    bool dead = false;
    bool _jumpWasDown = false;
    Entity _shadow{};
    float _floorY = 0.0f;
    std::vector<Entity> _dust;
    std::vector<float> _dustDensity;
    int _hitSoundId = -1;
    int _runningSoundId = -1;
    int _cameraId = -1;
};
