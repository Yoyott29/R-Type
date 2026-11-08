#ifndef CAMERAORBIT_HPP_
#define CAMERAORBIT_HPP_

#include "Camera.hpp"
#include "GameState.hpp"
#include "Net.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <cmath>
#include <string>
#include <vector>

#define DEGREES_TO_RADIANS (3.14159265f / 180.0f)

inline const std::vector<std::string> CameraOrbit_params = {"CenterX",    "CenterZ", "Radius",
                                                            "LookRadius", "Height",  "Speed"};

class CameraOrbit : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "CenterX")
            _centerX = std::stof(value);
        else if (name == "CenterZ")
            _centerZ = std::stof(value);
        else if (name == "Radius")
            _radius = std::stof(value);
        else if (name == "LookRadius")
            _lookRadius = std::stof(value);
        else if (name == "Height")
            _height = std::stof(value);
        else if (name == "Speed")
            _speed = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        // Client only: the server has no camera
        if (!Net::isClient(scene))
            return;
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        place(scene, entity);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        // Client only: the server has no camera
        if (!Net::isClient(scene))
            return;
        if (scene.getResource<GameState>().playing)
            return;
        _angle = std::fmod(_angle + _speed * FIXED_DELTA_TIME, 360.0f);
        place(scene, entity);
    }

  private:
    void place(Scene &scene, Entity entity) const
    {
        const float sine = std::sin(_angle * DEGREES_TO_RADIANS);
        const float cosine = std::cos(_angle * DEGREES_TO_RADIANS);
        Transform &position = scene.getComponent<Transform>(entity);
        Camera &camera = scene.getComponent<Camera>(entity);

        position.x = _centerX + sine * _radius;
        position.y = _height;
        position.z = _centerZ + cosine * _radius;
        camera.targetX = _centerX + sine * _lookRadius;
        camera.targetY = 0.0f;
        camera.targetZ = _centerZ + cosine * _lookRadius;
    }

    float _centerX = 0.0f;
    float _centerZ = 0.0f;
    float _radius = 100.0f;
    float _lookRadius = 0.0f;
    float _height = 50.0f;
    float _speed = 6.0f;
    float _angle = 0.0f;
};

#endif /* !CAMERAORBIT_HPP_ */
