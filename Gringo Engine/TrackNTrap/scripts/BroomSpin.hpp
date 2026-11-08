#ifndef BROOMSPIN_HPP_
#define BROOMSPIN_HPP_

#include "Hazards.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "Vector3.hpp"
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

#define BROOM_TOP_Y 0.5988f
#define BROOM_HEAD_BOTTOM_Y -0.7092f

#define BROOM_DEGREES_TO_RADIANS (3.14159265f / 180.0f)
#define BROOM_ASSET_START_ROTATION -90.0f

inline const std::vector<std::string> BroomSpin_params = {"Speed", "FloorGap", "SlowTime",
                                                          "SlowFactor"};

class BroomSpin : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "Speed")
            _speed = std::stof(value);
        else if (name == "FloorGap")
            _floorGap = std::stof(value);
        else if (name == "SlowTime")
            _slowTime = std::stof(value);
        else if (name == "SlowFactor")
            _slowFactor = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        Transform &broom = scene.getComponent<Transform>(entity);

        _pivotX = broom.x;
        _pivotY = broom.y + _floorGap + (BROOM_TOP_Y - BROOM_HEAD_BOTTOM_Y) * broom.scaleY;
        _pivotZ = broom.z;
        _baseRotationX = broom.rotationX;

        _angle = 360.0f * std::rand() / static_cast<float>(RAND_MAX);

        if (std::rand() % 2 == 0)
            _speed = -_speed;

        if (!scene.hasResource<Hazards>())
            scene.addResource<Hazards>();

        scene.getResource<Hazards>().add(entity, {Hazard::Effect::Slow, _slowTime, _slowFactor});
        apply(scene, entity);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        _angle = std::fmod(_angle + _speed * FIXED_DELTA_TIME, 360.0f);
        apply(scene, entity);
    }

  private:
    void apply(Scene &scene, Entity entity)
    {
        Transform &broom = scene.getComponent<Transform>(entity);
        broom.rotationX = _baseRotationX + _angle;

        Vector3 top = place({0.0f, BROOM_TOP_Y, 0.0f}, broom);
        broom.x = _pivotX - top.x;
        broom.y = _pivotY - top.y;
        broom.z = _pivotZ - top.z;
    }

    static Vector3 place(Vector3 point, const Transform &broom)
    {
        point.x *= broom.scaleX;
        point.y *= broom.scaleY;
        point.z *= broom.scaleZ;

        rotate(point.y, point.z, broom.rotationX);
        rotate(point.z, point.x, BROOM_ASSET_START_ROTATION);
        rotate(point.x, point.y, broom.rotationZ);
        rotate(point.z, point.x, -broom.rotationY);

        return point;
    }

    static void rotate(float &first, float &second, float degrees)
    {
        float sine = std::sin(degrees * BROOM_DEGREES_TO_RADIANS);
        float cosine = std::cos(degrees * BROOM_DEGREES_TO_RADIANS);
        float turnedFirst = cosine * first - sine * second;

        second = sine * first + cosine * second;
        first = turnedFirst;
    }

    float _speed = 200.0f;
    float _floorGap = 0.3f;
    float _slowTime = 3.0f;
    float _slowFactor = 0.5f;
    float _angle = 0.0f;
    float _baseRotationX = 0.0f;
    float _pivotX = 0.0f;
    float _pivotY = 0.0f;
    float _pivotZ = 0.0f;
};

#endif /* !BROOMSPIN_HPP_ */
