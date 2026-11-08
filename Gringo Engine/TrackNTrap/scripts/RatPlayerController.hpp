/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RatPlayerController
*/

#pragma once

#include "AudioSource.hpp"
#include "Cheeses.hpp"
#include "ChildOf.hpp"
#include "Collider3D.hpp"
#include "GameState.hpp"
#include "Hazards.hpp"
#include "KeyboardInput.hpp"
#include "Model3D.hpp"
#include "ParticleEmitter3D.hpp"
#include "ProjectComponentManager.hpp"
#include "RigidBody3D.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Traps.hpp"
#include "Velocity3D.hpp"
#include <algorithm>
#include <cmath>
#include <optional>

#define BODY_LENGTH 4.5f
#define BODY_WIDTH 1.1f

#define SHADOW_LENGTH 4.2f
#define SHADOW_WIDTH 1.7f
#define SHADOW_FORWARD 0.8f
#define SHADOW_THICKNESS 0.02f

#define SHADOW_LIFT 0.3f
#define SHADOW_FADE_HEIGHT 15.0f
#define SHADOW_COLOR ColorRGBA{0, 0, 0, 70}

#define STANDING_FRAME 0.58f

#define DUST_BOOST_DENSITY 2.0f
#define DUST_BOOST_COLOR ColorRGBA{40, 120, 255, 255}
#define DUST_FEET_BEHIND 0.8f
#define DUST_FEET_SIDE 0.35f
#define DUST_FEET_HEIGHT 0.3f

#define RAT_PLAYER_CONTROLLER_DEGREES_TO_RADIANS (3.14159265f / 180.0f)

std::vector<std::string> RatPlayerController_params = {
    "speed", "jump_force", "hit_sound", "running_sound", "camera", "boost_speed", "boost_time"};

class RatPlayerController : public GringoEntityScript
{
  public:
    void onStart(Scene &scene, Entity &entity) override
    {
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        spawn = scene.getComponent<Transform>(entity);

        scene.getComponent<Collider3D>(entity).onCollision = [this, &scene](Entity rat,
                                                                            Entity other) {
            if (scene.hasResource<Cheeses>() && scene.getResource<Cheeses>().eat(other)) {
                scene.getComponent<Collider3D>(other).layer = 0;
                _boost = _boostTime;
                return;
            }
            if (scene.hasResource<Traps>()) {
                Traps &traps = scene.getResource<Traps>();

                if (traps.covers(rat, other))
                    return;
                if (traps.hit(other))
                    scene.getComponent<Collider3D>(other).layer = 0;
            }
            if (scene.hasResource<Hazards>() && scene.getResource<Hazards>().has(other)) {
                suffer(scene.getResource<Hazards>().get(other));
                return;
            }
            dead = true;
        };
        createShadow(scene, entity);
        setIdle(scene.getComponent<Model3D>(entity), true);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (scene.getResource<GameState>().finished) {
            stopAtFinish(scene, entity);
            return;
        }
        if (!scene.getResource<GameState>().canMove)
            return;
        Transform &transform = scene.getComponent<Transform>(entity);
        const KeyboardInput &keyboard = scene.getResource<KeyboardInput>();
        RigidBody3D &body = scene.getComponent<RigidBody3D>(entity);
        Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);

        const bool respawned = dead;
        if (dead) {
            if (const std::optional<Entity> hit = findSound(scene, _hitSoundId))
                scene.getComponent<AudioSource>(*hit).play();
            transform = spawn;
            velocity = Velocity3D{};
            dead = false;
            _boost = 0.0f;
            _stun = 0.0f;
            _slow = 0.0f;
            ++scene.getResource<GameState>().respawns;
        }
        if (_hitSound) {
            _hitSound = false;
            if (const std::optional<Entity> hit = findSound(scene, _hitSoundId))
                scene.getComponent<AudioSource>(*hit).play();
        }
        _stun = std::max(_stun - FIXED_DELTA_TIME, 0.0f);
        _slow = std::max(_slow - FIXED_DELTA_TIME, 0.0f);
        const bool stunned = _stun > 0.0f;
        _boost = std::max(_boost - FIXED_DELTA_TIME, 0.0f);
        const bool boosted = _boost > 0.0f;
        scene.getResource<GameState>().boosted = boosted;

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
        const bool moving = length > 0.0f && !stunned && !respawned;
        const float movementSpeed =
            speed * (boosted ? _boostSpeed : 1.0f) * (_slow > 0.0f ? _slowFactor : 1.0f);
        velocity.dx = moving ? moveX / length * movementSpeed : 0.0f;
        velocity.dz = moving ? moveZ / length * movementSpeed : 0.0f;

        if (moving)
            transform.rotationY = std::atan2(moveX, -moveZ) * 180.0f / 3.14159265358979323846f;
        const float radians = transform.rotationY * RAT_PLAYER_CONTROLLER_DEGREES_TO_RADIANS;

        const bool jumpDown = keyboard.isDown(Key::Space);
        if (jumpDown && !_jumpWasDown && body.grounded && !stunned && !respawned)
            velocity.dy = jumpForce;
        _jumpWasDown = jumpDown;

        Collider3D &collider = scene.getComponent<Collider3D>(entity);
        fitCollider(collider, transform, radians);
        updateShadow(scene, transform, collider, body.grounded);
        setIdle(scene.getComponent<Model3D>(entity), !moving);
        const bool running = moving && body.grounded;
        updateDust(scene, entity, collider, running, boosted);
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
        else if (name == "boost_speed")
            _boostSpeed = std::stof(value);
        else if (name == "boost_time")
            _boostTime = std::stof(value);
    }

  private:
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

    static std::optional<Entity> findSound(Scene &scene, int editorId)
    {
        const std::optional<Entity> sound = scene.getResource<EditorEntityIds>().find(editorId);
        if (!sound || !scene.hasComponent<AudioSource>(*sound))
            return std::nullopt;
        return sound;
    }

    void suffer(const Hazard &hazard)
    {
        if (hazard.effect == Hazard::Effect::Stun) {
            if (_stun > 0.0f)
                return;
            _stun = hazard.duration;
        } else {
            if (_slow <= 0.0f)
                _hitSound = true;
            _slow = hazard.duration;
            _slowFactor = hazard.factor;
            return;
        }
        _hitSound = true;
    }

    void stopAtFinish(Scene &scene, Entity entity)
    {
        Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);

        velocity.dx = 0.0f;
        velocity.dz = 0.0f;
        _boost = 0.0f;
        scene.getResource<GameState>().boosted = false;
        setIdle(scene.getComponent<Model3D>(entity), true);
        updateDust(scene, entity, scene.getComponent<Collider3D>(entity), false, false);
        updateRunningSound(scene, false);
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

    static void setIdle(Model3D &model, bool idle)
    {
        model.animationFrame = idle ? STANDING_FRAME : -1.0f;
    }

    void createShadow(Scene &scene, Entity entity)
    {
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
        if (grounded)
            _floorY = feetY;
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

    void findDust(Scene &scene, Entity entity)
    {
        for (Entity child : scene.view<ChildOf, ParticleEmitter3D>()) {
            if (scene.getComponent<ChildOf>(child).parent != entity)
                continue;
            _dust.push_back(child);
            _dustDensity.push_back(scene.getComponent<ParticleEmitter3D>(child).density);
            _dustColor.push_back(scene.getComponent<ParticleEmitter3D>(child).color);
        }
    }

    void updateDust(Scene &scene, Entity entity, const Collider3D &collider, bool running,
                    bool boosted)
    {
        if (_dust.empty())
            findDust(scene, entity);
        const Transform &transform = scene.getComponent<Transform>(entity);
        const float radians = transform.rotationY * 3.14159265358979323846f / 180.0f;
        const float forwardX = std::sin(radians);
        const float forwardZ = -std::cos(radians);
        const float behind = DUST_FEET_BEHIND * transform.scaleX;
        const float side = DUST_FEET_SIDE * transform.scaleX;
        for (int i = 0; i < static_cast<int>(_dust.size()); ++i) {
            const float foot = i == 0 ? -side : side;
            Transform &position = scene.getComponent<Transform>(_dust[i]);
            position.x = transform.x - forwardX * behind - forwardZ * foot;
            position.y = transform.y + collider.offsetY - collider.height / 2.0f +
                         DUST_FEET_HEIGHT * transform.scaleY;
            position.z = transform.z - forwardZ * behind + forwardX * foot;
            scene.getComponent<ChildOf>(_dust[i]).lastParentPosition = transform;

            ParticleEmitter3D &dust = scene.getComponent<ParticleEmitter3D>(_dust[i]);
            dust.density = boosted ? _dustDensity[i] * DUST_BOOST_DENSITY : _dustDensity[i];
            dust.color = boosted ? DUST_BOOST_COLOR : _dustColor[i];
            if (running && !dust.isEmitting())
                dust.start();
            else if (!running && dust.isEmitting())
                dust.stop();
        }
    }

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
    std::vector<ColorRGBA> _dustColor;
    int _hitSoundId = -1;
    float _boostSpeed = 2.0f;
    float _boostTime = 2.0f;
    float _boost = 0.0f;
    float _stun = 0.0f;
    float _slow = 0.0f;
    float _slowFactor = 1.0f;
    bool _hitSound = false;
    int _runningSoundId = -1;
    int _cameraId = -1;
};
