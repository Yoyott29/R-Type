#ifndef SPATULASLAP_HPP_
#define SPATULASLAP_HPP_

#include "Collider3D.hpp"
#include "ParticleEmitter3D.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>
#include <vector>

#define SPATULA_MIN_X -113.25f
#define SPATULA_MAX_X 120.158f
#define SPATULA_MIN_Y 15.567f
#define SPATULA_MAX_Y 781.799f
#define SPATULA_MIN_Z -28.099f
#define SPATULA_MAX_Z 13.69f

// Middle of the spatula head in spatula.glb: where the particles burst out when it hits the floor
#define SPATULA_HEAD_CENTER_X 3.454f
#define SPATULA_HEAD_CENTER_Y 541.273f
#define SPATULA_HEAD_CENTER_Z -7.204f

#define SPATULA_DEGREES_TO_RADIANS (3.14159265f / 180.0f)
#define SPATULA_UPRIGHT_ANGLE 90.0f

#define SPATULA_ASSET_YAW -90.0f

// "BurstTime": seconds the spatula's ParticleEmitter3D emits under the head on each slap
inline const std::vector<std::string> SpatulaSlap_params = {"MinDelay", "MaxDelay", "SlapTime",
                                                            "HoldTime", "RaiseTime", "BurstTime"};

// Stands up at 90 degrees, waits a random time, then slams flat on the floor and slowly raises
// back. With RotationY = 0 it falls towards +X: turn the entity to choose where it slaps.
class SpatulaSlap : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "MinDelay")
            _minDelay = std::stof(value);
        else if (name == "MaxDelay")
            _maxDelay = std::stof(value);
        else if (name == "SlapTime")
            _slapTime = std::stof(value);
        else if (name == "HoldTime")
            _holdTime = std::stof(value);
        else if (name == "RaiseTime")
            _raiseTime = std::stof(value);
        else if (name == "BurstTime")
            _burstTime = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        static bool seeded = false;

        if (!seeded) {
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            seeded = true;
        }
        _baseRotationX = scene.getComponent<Transform>(entity).rotationX;
        _state = State::Waiting;
        _timer = random(_minDelay, _maxDelay);
        apply(scene, entity, SPATULA_UPRIGHT_ANGLE);
        createImpact(scene, entity);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        _timer -= FIXED_DELTA_TIME;
        switch (_state) {
        case State::Waiting:
            if (_timer <= 0.0f)
                next(State::Slapping, _slapTime);
            break;
        case State::Slapping:
            if (_timer <= 0.0f) {
                next(State::Holding, _holdTime);
                _burst = _burstTime;
            }
            break;
        case State::Holding:
            if (_timer <= 0.0f)
                next(State::Raising, _raiseTime);
            break;
        case State::Raising:
            if (_timer <= 0.0f)
                next(State::Waiting, random(_minDelay, _maxDelay));
            break;
        }
        apply(scene, entity, angle());
        updateImpact(scene, entity);
    }

  private:
    enum class State { Waiting, Slapping, Holding, Raising };

    // The emitter follows its entity's Transform, the spatula's is the handle: the emitter moves to
    // an entity of its own, placed under the head on each slap
    void createImpact(Scene &scene, Entity spatula)
    {
        if (!scene.hasComponent<ParticleEmitter3D>(spatula))
            return;
        ParticleEmitter3D emitter = scene.getComponent<ParticleEmitter3D>(spatula);

        emitter.stop();
        emitter.particles.clear();
        scene.removeComponent<ParticleEmitter3D>(spatula);
        _impact = scene.createEntity();
        scene.addComponent(*_impact, Transform{});
        scene.addComponent(*_impact, emitter);
    }

    void updateImpact(Scene &scene, Entity spatula)
    {
        if (!_impact)
            return;
        ParticleEmitter3D &emitter = scene.getComponent<ParticleEmitter3D>(*_impact);

        if (_burst > 0.0f && !emitter.isEmitting()) {
            const Transform &transform = scene.getComponent<Transform>(spatula);
            Transform &impact = scene.getComponent<Transform>(*_impact);
            float head[3] = {SPATULA_HEAD_CENTER_X, SPATULA_HEAD_CENTER_Y, SPATULA_HEAD_CENTER_Z};

            place(head, transform);
            impact.x = transform.x + head[0];
            impact.y = transform.y + head[1];
            impact.z = transform.z + head[2];
            emitter.start();
        }
        _burst -= FIXED_DELTA_TIME;
        if (_burst <= 0.0f && emitter.isEmitting())
            emitter.stop();
    }

    void next(State state, float duration)
    {
        _state = state;
        _duration = std::max(duration, FIXED_DELTA_TIME);
        _timer = _duration;
    }

    // Angle between the blade and the floor: 90 standing, 0 flat
    float angle() const
    {
        const float progress = std::clamp(1.0f - _timer / _duration, 0.0f, 1.0f);

        switch (_state) {
        case State::Slapping:
            // Accelerates like a swing, hits the floor at full speed
            return SPATULA_UPRIGHT_ANGLE * (1.0f - progress * progress);
        case State::Holding:
            return 0.0f;
        case State::Raising:
            // Slows down when getting back up
            return SPATULA_UPRIGHT_ANGLE * (1.0f - (1.0f - progress) * (1.0f - progress));
        default:
            return SPATULA_UPRIGHT_ANGLE;
        }
    }

    void apply(Scene &scene, Entity &entity, float upAngle)
    {
        Transform &spatula = scene.getComponent<Transform>(entity);
        Collider3D &collider = scene.getComponent<Collider3D>(entity);
        float minimum[3] = {0.0f, 0.0f, 0.0f};
        float maximum[3] = {0.0f, 0.0f, 0.0f};

        spatula.rotationX = _baseRotationX - (SPATULA_UPRIGHT_ANGLE - upAngle);
        for (int corner = 0; corner < 8; ++corner) {
            float point[3] = {corner & 1 ? SPATULA_MAX_X : SPATULA_MIN_X,
                              corner & 2 ? SPATULA_MAX_Y : SPATULA_MIN_Y,
                              corner & 4 ? SPATULA_MAX_Z : SPATULA_MIN_Z};

            place(point, spatula);
            for (int axis = 0; axis < 3; ++axis) {
                minimum[axis] = corner == 0 ? point[axis] : std::min(minimum[axis], point[axis]);
                maximum[axis] = corner == 0 ? point[axis] : std::max(maximum[axis], point[axis]);
            }
        }
        collider.offsetX = (minimum[0] + maximum[0]) / 2.0f;
        collider.offsetY = (minimum[1] + maximum[1]) / 2.0f;
        collider.offsetZ = (minimum[2] + maximum[2]) / 2.0f;
        collider.width = maximum[0] - minimum[0];
        collider.height = maximum[1] - minimum[1];
        collider.depth = maximum[2] - minimum[2];
    }

    // Model point to world offset, in the renderer's order: scale, X, asset yaw, Z, then yaw
    static void place(float (&point)[3], const Transform &spatula)
    {
        point[0] *= spatula.scaleX;
        point[1] *= spatula.scaleY;
        point[2] *= spatula.scaleZ;
        rotate(point[1], point[2], spatula.rotationX);
        rotate(point[2], point[0], SPATULA_ASSET_YAW);
        rotate(point[0], point[1], spatula.rotationZ);
        rotate(point[2], point[0], -spatula.rotationY);
    }

    // Turns (first, second) by degrees, from first towards second
    static void rotate(float &first, float &second, float degrees)
    {
        const float sine = std::sin(degrees * SPATULA_DEGREES_TO_RADIANS);
        const float cosine = std::cos(degrees * SPATULA_DEGREES_TO_RADIANS);
        const float turnedFirst = cosine * first - sine * second;

        second = sine * first + cosine * second;
        first = turnedFirst;
    }

    static float random(float min, float max)
    {
        return min + (max - min) * static_cast<float>(std::rand()) / RAND_MAX;
    }

    float _minDelay = 1.0f;
    float _maxDelay = 3.0f;
    float _slapTime = 0.15f;
    float _holdTime = 0.4f;
    float _raiseTime = 1.0f;
    float _burstTime = 0.1f;
    float _burst = 0.0f;
    std::optional<Entity> _impact;
    float _baseRotationX = 0.0f;
    float _timer = 0.0f;
    float _duration = 1.0f;
    State _state = State::Waiting;
};

#endif /* !SPATULASLAP_HPP_ */
