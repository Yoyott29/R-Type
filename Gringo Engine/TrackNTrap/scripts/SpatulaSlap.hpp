#ifndef SPATULASLAP_HPP_
#define SPATULASLAP_HPP_

#include "Hazards.hpp"
#include "ParticleEmitter3D.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Vector3.hpp"
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#define SPATULA_HEAD_CENTER_X 3.454f
#define SPATULA_HEAD_CENTER_Y 541.273f
#define SPATULA_HEAD_CENTER_Z -7.204f

#define SPATULA_DEGREES_TO_RADIANS (3.14159265f / 180.0f)
#define SPATULA_UPRIGHT_ANGLE 90.0f

#define SPATULA_ASSET_YAW -90.0f

inline const std::vector<std::string> SpatulaSlap_params = {
    "MinDelay", "MaxDelay", "SlapTime", "HoldTime", "RaiseTime", "BurstTime", "StunTime"};

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
        else if (name == "StunTime")
            _stunTime = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        _baseRotationX = scene.getComponent<Transform>(entity).rotationX;
        _state = State::Waiting;
        _timer = randomDelay();
        apply(scene, entity, SPATULA_UPRIGHT_ANGLE);
        createImpact(scene, entity);
        if (!scene.hasResource<Hazards>())
            scene.addResource<Hazards>();
        scene.getResource<Hazards>().add(entity, {Hazard::Effect::Stun, _stunTime});
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
                next(State::Waiting, randomDelay());
            break;
        }
        apply(scene, entity, angle());
        updateImpact(scene, entity);
    }

  private:
    enum class State { Waiting, Slapping, Holding, Raising };

    void createImpact(Scene &scene, Entity spatula)
    {
        if (!scene.hasComponent<ParticleEmitter3D>(spatula))
            return;
        ParticleEmitter3D emitter = scene.getComponent<ParticleEmitter3D>(spatula);

        emitter.stop();
        emitter.particles.clear();
        scene.removeComponent<ParticleEmitter3D>(spatula);
        _impact = scene.createEntity();
        _hasImpact = true;
        scene.addComponent(_impact, Transform{});
        scene.addComponent(_impact, emitter);
    }

    void updateImpact(Scene &scene, Entity spatula)
    {
        if (!_hasImpact)
            return;
        ParticleEmitter3D &emitter = scene.getComponent<ParticleEmitter3D>(_impact);

        if (_burst > 0.0f && !emitter.isEmitting()) {
            Transform &transform = scene.getComponent<Transform>(spatula);
            Transform &impact = scene.getComponent<Transform>(_impact);
            Vector3 head = place(
                {SPATULA_HEAD_CENTER_X, SPATULA_HEAD_CENTER_Y, SPATULA_HEAD_CENTER_Z}, transform);

            impact.x = transform.x + head.x;
            impact.y = transform.y + head.y;
            impact.z = transform.z + head.z;
            emitter.start();
        }
        _burst -= FIXED_DELTA_TIME;
        if (_burst <= 0.0f && emitter.isEmitting())
            emitter.stop();
    }

    float randomDelay()
    {
        return _minDelay + (_maxDelay - _minDelay) * std::rand() / static_cast<float>(RAND_MAX);
    }

    void next(State state, float duration)
    {
        _state = state;
        _duration = duration;
        if (_duration < FIXED_DELTA_TIME)
            _duration = FIXED_DELTA_TIME;
        _timer = _duration;
    }

    float angle() const
    {
        float progress = 1.0f - _timer / _duration;
        if (progress < 0.0f)
            progress = 0.0f;
        if (progress > 1.0f)
            progress = 1.0f;

        switch (_state) {
        case State::Slapping:
            return SPATULA_UPRIGHT_ANGLE * (1.0f - progress * progress);
        case State::Holding:
            return 0.0f;
        case State::Raising:
            return SPATULA_UPRIGHT_ANGLE * (1.0f - (1.0f - progress) * (1.0f - progress));
        default:
            return SPATULA_UPRIGHT_ANGLE;
        }
    }

    void apply(Scene &scene, Entity &entity, float upAngle)
    {
        scene.getComponent<Transform>(entity).rotationX =
            _baseRotationX - (SPATULA_UPRIGHT_ANGLE - upAngle);
    }

    static Vector3 place(Vector3 point, const Transform &spatula)
    {
        point.x *= spatula.scaleX;
        point.y *= spatula.scaleY;
        point.z *= spatula.scaleZ;
        rotate(point.y, point.z, spatula.rotationX);
        rotate(point.z, point.x, SPATULA_ASSET_YAW);
        rotate(point.x, point.y, spatula.rotationZ);
        rotate(point.z, point.x, -spatula.rotationY);
        return point;
    }

    static void rotate(float &first, float &second, float degrees)
    {
        float sine = std::sin(degrees * SPATULA_DEGREES_TO_RADIANS);
        float cosine = std::cos(degrees * SPATULA_DEGREES_TO_RADIANS);
        float turnedFirst = cosine * first - sine * second;

        second = sine * first + cosine * second;
        first = turnedFirst;
    }

    float _minDelay = 1.0f;
    float _maxDelay = 3.0f;
    float _slapTime = 0.15f;
    float _holdTime = 0.4f;
    float _raiseTime = 1.0f;
    float _burstTime = 0.1f;
    float _stunTime = 2.0f;
    float _burst = 0.0f;
    Entity _impact{};
    bool _hasImpact = false;
    float _baseRotationX = 0.0f;
    float _timer = 0.0f;
    float _duration = 1.0f;
    State _state = State::Waiting;
};

#endif /* !SPATULASLAP_HPP_ */
