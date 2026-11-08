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
#include "Model3D.hpp"
#include "Net.hpp"
#include "NetComponents.hpp"
#include "ParticleEmitter3D.hpp"
#include "ProjectComponentManager.hpp"
#include "RigidBody3D.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Traps.hpp"
#include "Velocity3D.hpp"
#include <cmath>

#define BODY_LENGTH 4.5f
#define BODY_WIDTH 1.1f

#define SHADOW_LENGTH 4.2f
#define SHADOW_WIDTH 1.7f
#define SHADOW_FORWARD 0.8f
#define SHADOW_THICKNESS 0.02f

#define SHADOW_LIFT 0.3f
#define SHADOW_COLOR ColorRGBA{0, 0, 0, 70}

#define STANDING_FRAME 0.58f

#define DUST_BOOST_DENSITY 2.0f
#define DUST_BOOST_COLOR ColorRGBA{40, 120, 255, 255}
#define DUST_FEET_BEHIND 0.8f
#define DUST_FEET_SIDE 0.35f
#define DUST_FEET_HEIGHT 0.3f

#define RAT_PLAYER_CONTROLLER_DEGREES_TO_RADIANS (3.14159265f / 180.0f)

// A small box above each rat, of its player's color, so the rats can be told apart. Sizes are
// multiplied by the rat's scale. The colors stay distinct for color-blind players
#define BOX_SIZE 0.6f
#define BOX_GAP 0.3f
#define BOX_COLOR_1 ColorRGBA{0, 114, 178, 255}
#define BOX_COLOR_2 ColorRGBA{230, 159, 0, 255}
#define BOX_COLOR_3 ColorRGBA{204, 121, 167, 255}
#define BOX_COLOR_4 ColorRGBA{0, 158, 115, 255}

std::vector<std::string> RatPlayerController_params = {
    "speed", "jump_force", "hit_sound", "running_sound", "boost_speed", "boost_time"};

class RatPlayerController : public GringoEntityScript
{
  public:
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
        else if (name == "boost_speed")
            _boostSpeed = std::stof(value);
        else if (name == "boost_time")
            _boostTime = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        if (Net::isClient(scene))
            startClient(scene, entity);
        else
            startServer(scene, entity);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (Net::isClient(scene))
            updateClient(scene, entity);
        else
            updateServer(scene, entity);
    }

  private:
    // ---------- Server: the real rat ----------

    void startServer(Scene &scene, Entity entity)
    {
        spawn = scene.getComponent<Transform>(entity);
        if (!scene.hasComponent<RatStatus>(entity))
            scene.addComponent(entity, RatStatus{});

        // Only called for triggers: the floor is solid, a cheese is eaten, a spatula stuns, a
        // broom slows, every other trigger sends the rat back to the start except a trap it just
        // sent
        scene.getComponent<Collider3D>(entity).onCollision =
            [this, &scene](Entity rat, Entity other) { touched(scene, rat, other); };
    }

    void touched(Scene &scene, Entity rat, Entity other)
    {
        if (scene.hasResource<Cheeses>() && scene.getResource<Cheeses>().eat(other)) {
            // Touches nothing anymore until it's gone, or it would kill the rat next step
            scene.getComponent<Collider3D>(other).layer = 0;
            _boost = _boostTime;
            return;
        }
        if (scene.hasResource<Traps>()) {
            Traps &traps = scene.getResource<Traps>();

            if (traps.covers(rat, other))
                return;
            // A trap stops on the rat it caught
            if (traps.hit(other))
                scene.getComponent<Collider3D>(other).layer = 0;
        }
        if (scene.hasResource<Hazards>() && scene.getResource<Hazards>().has(other)) {
            suffer(scene.getResource<Hazards>().get(other));
            return;
        }
        // Another rat is solid, not a trigger: never here
        dead = true;
    }

    // A stun doesn't grow while it lasts, or the rat could stay stuck under a spatula; a slow
    // starts again from the full time
    void suffer(Hazard hazard)
    {
        if (hazard.effect == Hazard::Effect::Stun) {
            if (_stun > 0.0f)
                return;
            _stun = hazard.duration;
            _hit = true;
            return;
        }
        if (_slow <= 0.0f)
            _hit = true;
        _slow = hazard.duration;
        _slowFactor = hazard.factor;
    }

    void updateServer(Scene &scene, Entity entity)
    {
        GameState &state = scene.getResource<GameState>();
        Transform &transform = scene.getComponent<Transform>(entity);
        RigidBody3D &body = scene.getComponent<RigidBody3D>(entity);
        Velocity3D &velocity = scene.getComponent<Velocity3D>(entity);
        RatStatus &status = scene.getComponent<RatStatus>(entity);

        status.grounded = body.grounded;
        // In the lobby, during the countdown and after the race: stands still
        if (!state.canMove) {
            velocity.dx = 0.0f;
            velocity.dz = 0.0f;
            status.moving = false;
            status.boosted = false;
            return;
        }
        bool respawned = dead;
        if (dead) {
            transform = spawn;
            velocity = Velocity3D{};
            dead = false;
            _boost = 0.0f;
            _stun = 0.0f;
            _slow = 0.0f;
            status.respawns += 1;
            _hit = true;
        }
        if (_hit) {
            status.hits += 1;
            _hit = false;
        }
        _stun = countDown(_stun);
        _slow = countDown(_slow);
        _boost = countDown(_boost);
        bool stunned = _stun > 0.0f;
        status.boosted = _boost > 0.0f;

        // The keys of the player who owns this rat
        ActionInput input;
        if (scene.hasComponent<RemoteInput>(entity))
            input = scene.getComponent<RemoteInput>(entity).input;

        // Z goes the way the player's camera looks, S the other way
        float camera = Net::cameraAngle(input) * RAT_PLAYER_CONTROLLER_DEGREES_TO_RADIANS;
        float viewX = std::sin(camera);
        float viewZ = -std::cos(camera);
        float moveX = 0.0f;
        float moveZ = 0.0f;
        if (input.isActive(ACTION_FORWARD)) {
            moveX += viewX;
            moveZ += viewZ;
        }
        if (input.isActive(ACTION_BACK)) {
            moveX -= viewX;
            moveZ -= viewZ;
        }
        float length = std::hypot(moveX, moveZ);
        bool moving = length > 0.0f && !stunned && !respawned;
        float movementSpeed = speed;
        if (status.boosted)
            movementSpeed *= _boostSpeed;
        if (_slow > 0.0f)
            movementSpeed *= _slowFactor;
        velocity.dx = 0.0f;
        velocity.dz = 0.0f;
        if (moving) {
            velocity.dx = moveX / length * movementSpeed;
            velocity.dz = moveZ / length * movementSpeed;
            // Faces where it runs: forward is (sin, -cos) of its rotation
            transform.rotationY =
                std::atan2(moveX, -moveZ) / RAT_PLAYER_CONTROLLER_DEGREES_TO_RADIANS;
        }
        status.moving = moving;

        bool jumpDown = input.isActive(ACTION_JUMP);
        if (jumpDown && !_jumpWasDown && body.grounded && !stunned && !respawned)
            velocity.dy = jumpForce;
        _jumpWasDown = jumpDown;
        fitCollider(scene.getComponent<Collider3D>(entity), transform);
    }

    static float countDown(float seconds)
    {
        seconds -= FIXED_DELTA_TIME;
        if (seconds < 0.0f)
            seconds = 0.0f;
        return seconds;
    }

    // The box around the body, turned like the rat
    static void fitCollider(Collider3D &collider, Transform &transform)
    {
        float radians = transform.rotationY * RAT_PLAYER_CONTROLLER_DEGREES_TO_RADIANS;
        float length = BODY_LENGTH * transform.scaleX;
        float width = BODY_WIDTH * transform.scaleX;
        float sine = std::abs(std::sin(radians));
        float cosine = std::abs(std::cos(radians));

        collider.width = length * sine + width * cosine;
        collider.depth = length * cosine + width * sine;
    }

    // ---------- Client: the copy of a rat, sent by the server ----------

    void startClient(Scene &scene, Entity entity)
    {
        // The server moves it: no gravity nor pushing here
        if (scene.hasComponent<RigidBody3D>(entity)) {
            scene.getComponent<RigidBody3D>(entity).isStatic = true;
            scene.getComponent<RigidBody3D>(entity).gravityScale = 0.0f;
        }
        createShadow(scene, entity);
        createDust(scene, entity);
        createBox(scene, entity);
        setIdle(scene.getComponent<Model3D>(entity), true);
    }

    // The colored box, a child of the rat: it follows the rat and is destroyed with it. Its
    // Transform is relative to the rat: straight above the middle of the body
    static void createBox(Scene &scene, Entity rat)
    {
        Transform &ratTransform = scene.getComponent<Transform>(rat);
        Collider3D &collider = scene.getComponent<Collider3D>(rat);
        float size = BOX_SIZE * ratTransform.scaleX;
        Entity box = scene.createEntity();
        Transform position;
        Model3D model;
        ChildOf link;

        position.y =
            collider.offsetY + collider.height / 2.0f + BOX_GAP * ratTransform.scaleX + size / 2.0f;
        model.primitive = "Cube";
        model.color = boxColor(Net::slot(scene, rat));
        model.width = size;
        model.height = size;
        model.depth = size;
        link.parent = rat;
        scene.addComponent(box, position);
        scene.addComponent(box, model);
        scene.addComponent(box, link);
    }

    // Rat1 blue, Rat2 orange, Rat3 pink, Rat4 teal
    static ColorRGBA boxColor(int slot)
    {
        if (slot == 1)
            return BOX_COLOR_1;
        if (slot == 2)
            return BOX_COLOR_2;
        if (slot == 3)
            return BOX_COLOR_3;
        return BOX_COLOR_4;
    }

    void updateClient(Scene &scene, Entity entity)
    {
        if (!scene.hasComponent<RatStatus>(entity))
            return;
        RatStatus &status = scene.getComponent<RatStatus>(entity);
        Transform &transform = scene.getComponent<Transform>(entity);
        Collider3D &collider = scene.getComponent<Collider3D>(entity);
        bool running = status.moving && status.grounded;

        setIdle(scene.getComponent<Model3D>(entity), !status.moving);
        updateShadow(scene, transform, collider, status.grounded);
        updateDust(scene, transform, collider, running, status.boosted);

        // Only the rat of this player makes sounds
        Entity mine;
        if (!Net::findMyRat(scene, mine) || mine != entity)
            return;
        updateRunningSound(scene, running);
        if (status.hits != _hits) {
            _hits = status.hits;
            playSound(scene, _hitSoundId);
        }
    }

    static bool findSound(Scene &scene, int editorId, Entity &sound)
    {
        EditorEntityIds &ids = scene.getResource<EditorEntityIds>();

        if (ids.entities.count(editorId) == 0)
            return false;
        sound = ids.entities.at(editorId);
        return scene.hasComponent<AudioSource>(sound);
    }

    static void playSound(Scene &scene, int editorId)
    {
        Entity sound;
        if (findSound(scene, editorId, sound))
            scene.getComponent<AudioSource>(sound).play();
    }

    void updateRunningSound(Scene &scene, bool running)
    {
        Entity entity;
        if (!findSound(scene, _runningSoundId, entity))
            return;
        AudioSource &sound = scene.getComponent<AudioSource>(entity);
        if (running && !sound.isPlaying())
            sound.play();
        else if (!running && sound.isPlaying())
            sound.stop();
    }

    // Standing still: the run animation held on a frame with the feet on the ground
    static void setIdle(Model3D &model, bool idle)
    {
        if (idle)
            model.animationFrame = STANDING_FRAME;
        else
            model.animationFrame = -1.0f;
    }

    void createShadow(Scene &scene, Entity entity)
    {
        Transform transform = scene.getComponent<Transform>(entity);
        Collider3D &collider = scene.getComponent<Collider3D>(entity);
        Model3D model;

        _floorY = transform.y + collider.offsetY - collider.height / 2.0f;
        _shadow = scene.createEntity();
        scene.addComponent<Transform>(_shadow, transform);
        model.primitive = "Sphere";
        model.color = SHADOW_COLOR;
        scene.addComponent<Model3D>(_shadow, model);
        updateShadow(scene, transform, collider, true);
    }

    // A flat dark ellipse on the floor, under the rat: shows where it will land while jumping
    void updateShadow(Scene &scene, Transform &transform, Collider3D &collider, bool grounded)
    {
        float feetY = transform.y + collider.offsetY - collider.height / 2.0f;
        if (grounded)
            _floorY = feetY;
        float radians = transform.rotationY * RAT_PLAYER_CONTROLLER_DEGREES_TO_RADIANS;
        float forward = SHADOW_FORWARD * transform.scaleX;
        Transform &shadow = scene.getComponent<Transform>(_shadow);

        shadow.x = transform.x + std::sin(radians) * forward;
        shadow.y = _floorY + SHADOW_LIFT;
        shadow.z = transform.z - std::cos(radians) * forward;
        shadow.rotationY = transform.rotationY;
        shadow.scaleX = SHADOW_WIDTH * transform.scaleX;
        shadow.scaleY = SHADOW_THICKNESS;
        shadow.scaleZ = SHADOW_LENGTH * transform.scaleX;
    }

    // The ParticleEmitter3D of the prefab goes to two entities of their own, one under each back
    // foot: an emitter sends its particles from its entity's position
    void createDust(Scene &scene, Entity entity)
    {
        if (!scene.hasComponent<ParticleEmitter3D>(entity))
            return;
        ParticleEmitter3D emitter = scene.getComponent<ParticleEmitter3D>(entity);

        scene.removeComponent<ParticleEmitter3D>(entity);
        emitter.stop();
        _dustDensity = emitter.density;
        _dustColor = emitter.color;
        _leftDust = scene.createEntity();
        _rightDust = scene.createEntity();
        scene.addComponent(_leftDust, Transform{});
        scene.addComponent(_rightDust, Transform{});
        scene.addComponent(_leftDust, emitter);
        scene.addComponent(_rightDust, emitter);
        _hasDust = true;
    }

    void updateDust(Scene &scene, Transform &transform, Collider3D &collider, bool running,
                    bool boosted)
    {
        if (!_hasDust)
            return;
        placeDust(scene, _leftDust, transform, collider, -1.0f, running, boosted);
        placeDust(scene, _rightDust, transform, collider, 1.0f, running, boosted);
    }

    // side: -1 for the left foot, 1 for the right one
    void placeDust(Scene &scene, Entity dust, Transform &transform, Collider3D &collider,
                   float side, bool running, bool boosted)
    {
        float radians = transform.rotationY * RAT_PLAYER_CONTROLLER_DEGREES_TO_RADIANS;
        float forwardX = std::sin(radians);
        float forwardZ = -std::cos(radians);
        float behind = DUST_FEET_BEHIND * transform.scaleX;
        float foot = side * DUST_FEET_SIDE * transform.scaleX;
        Transform &position = scene.getComponent<Transform>(dust);
        ParticleEmitter3D &emitter = scene.getComponent<ParticleEmitter3D>(dust);

        position.x = transform.x - forwardX * behind - forwardZ * foot;
        position.y = transform.y + collider.offsetY - collider.height / 2.0f +
                     DUST_FEET_HEIGHT * transform.scaleY;
        position.z = transform.z - forwardZ * behind + forwardX * foot;
        emitter.density = _dustDensity;
        emitter.color = _dustColor;
        if (boosted) {
            emitter.density = _dustDensity * DUST_BOOST_DENSITY;
            emitter.color = DUST_BOOST_COLOR;
        }
        if (running && !emitter.isEmitting())
            emitter.start();
        else if (!running && emitter.isEmitting())
            emitter.stop();
    }

    // Settings
    float speed = 6.0f;
    float jumpForce = 5.0f;
    float _boostSpeed = 2.0f;
    float _boostTime = 2.0f;
    int _hitSoundId = -1;
    int _runningSoundId = -1;

    // Server
    Transform spawn;
    bool dead = false;
    bool _hit = false;
    bool _jumpWasDown = false;
    float _boost = 0.0f;
    float _stun = 0.0f;
    float _slow = 0.0f;
    float _slowFactor = 1.0f;

    // Client
    int _hits = 0;
    Entity _shadow{};
    float _floorY = 0.0f;
    bool _hasDust = false;
    Entity _leftDust{};
    Entity _rightDust{};
    float _dustDensity = 0.0f;
    ColorRGBA _dustColor{};
};
