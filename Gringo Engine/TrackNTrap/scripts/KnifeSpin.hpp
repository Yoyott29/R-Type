#ifndef KNIFESPIN_HPP_
#define KNIFESPIN_HPP_

#include "Net.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#define DEGREES_TO_RADIANS (3.14159265f / 180.0f)

#define MIDDLE_X 0.0845f
#define MIDDLE_Y 0.0325f

inline const std::vector<std::string> KnifeSpin_params = {"RotationSpeed", "MoveSpeed", "MoveRange",
                                                          "MoveAngle"};

class KnifeSpin : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "RotationSpeed")
            _rotationSpeed = std::stof(value);
        else if (name == "MoveSpeed")
            _moveSpeed = std::stof(value);
        else if (name == "MoveRange")
            _moveRange = std::stof(value);
        else if (name == "MoveAngle")
            _moveAngle = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        // The server moves it, the clients show what it sends
        if (Net::isClient(scene))
            return;
        Transform &knife = scene.getComponent<Transform>(entity);
        const Transform middle = turned(MIDDLE_X, MIDDLE_Y, knife);

        _centerX = knife.x + middle.x;
        _centerY = knife.y + middle.y;
        _centerZ = knife.z;
        _moveX = std::sin(_moveAngle * DEGREES_TO_RADIANS);
        _moveZ = std::cos(_moveAngle * DEGREES_TO_RADIANS);
        _moveDirection = std::rand() % 2 == 0 ? -1.0f : 1.0f;
        _offset = randomBetween(-_moveRange, _moveRange);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        // The server moves it, the clients show what it sends
        if (Net::isClient(scene))
            return;
        _timer -= FIXED_DELTA_TIME;
        if (_timer <= 0.0f) {
            _spinX = randomBetween(-_rotationSpeed, _rotationSpeed);
            _spinY = randomBetween(-_rotationSpeed, _rotationSpeed);
            _spinZ = randomBetween(-_rotationSpeed, _rotationSpeed);
            _timer = randomBetween(1.0f, 2.5f);
        }

        Transform &knife = scene.getComponent<Transform>(entity);

        knife.rotationX = std::fmod(knife.rotationX + _spinX * FIXED_DELTA_TIME, 360.0f);
        knife.rotationY = std::fmod(knife.rotationY + _spinY * FIXED_DELTA_TIME, 360.0f);
        knife.rotationZ = std::fmod(knife.rotationZ + _spinZ * FIXED_DELTA_TIME, 360.0f);

        const Transform middle = turned(MIDDLE_X, MIDDLE_Y, knife);

        if (_offset < -_moveRange) {
            _offset = -_moveRange;
            _moveDirection = 1.0f;
        } else if (_offset > _moveRange) {
            _offset = _moveRange;
            _moveDirection = -1.0f;
        }
        _offset += _moveDirection * _moveSpeed * FIXED_DELTA_TIME;

        knife.x = _centerX + _offset * _moveX - middle.x;
        knife.y = _centerY - middle.y;
        knife.z = _centerZ + _offset * _moveZ;
    }

  private:
    static float randomBetween(float min, float max)
    {
        return min + (max - min) * std::rand() / static_cast<float>(RAND_MAX);
    }

    static void rotate(float &first, float &second, float degrees)
    {
        const float sine = std::sin(degrees * DEGREES_TO_RADIANS);
        const float cosine = std::cos(degrees * DEGREES_TO_RADIANS);
        const float turnedFirst = cosine * first - sine * second;

        second = sine * first + cosine * second;
        first = turnedFirst;
    }

    static Transform turned(float x, float y, const Transform &knife)
    {
        Transform point;

        point.y = y * knife.scaleX;
        point.z = 0.0f;
        rotate(point.y, point.z, knife.rotationX);
        point.x = -point.z;
        point.z = x * knife.scaleX;
        rotate(point.x, point.y, knife.rotationZ);
        rotate(point.z, point.x, -knife.rotationY);
        return point;
    }

    float _rotationSpeed = 90.0f;
    float _moveSpeed = 10.0f;
    float _moveRange = 10.0f;
    float _moveAngle = 0.0f;
    float _moveX = 0.0f;
    float _moveZ = 1.0f;
    float _offset = 0.0f;
    float _timer = 0.0f;
    float _spinX = 0.0f;
    float _spinY = 0.0f;
    float _spinZ = 0.0f;
    float _centerX = 0.0f;
    float _centerY = 0.0f;
    float _centerZ = 0.0f;
    float _moveDirection = 1.0f;
};

#endif /* !KNIFESPIN_HPP_ */
