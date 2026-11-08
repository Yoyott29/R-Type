#ifndef RATCAMERA_HPP_
#define RATCAMERA_HPP_

#include "Camera.hpp"
#include "GameState.hpp"
#include "IRenderer.hpp"
#include "ProjectComponentManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#define RAT_CAMERA_MIN_PITCH 5.0f
#define RAT_CAMERA_MAX_PITCH 80.0f
#define RAT_CAMERA_ZOOM_SPEED 4.0f
#define RAT_CAMERA_DEGREES_TO_RADIANS (3.14159265f / 180.0f)

inline const std::vector<std::string> RatCamera_params = {
    "rat", "distance", "pitch", "target_height", "sensitivity", "boost_distance"};

class RatCamera : public GringoEntityScript
{
  public:
    void setParameter(const std::string &name, const std::string &value) override
    {
        if (name == "rat")
            _ratId = std::stoi(value);
        else if (name == "distance")
            _distance = std::stof(value);
        else if (name == "pitch")
            _pitch = std::stof(value);
        else if (name == "target_height")
            _targetHeight = std::stof(value);
        else if (name == "sensitivity")
            _sensitivity = std::stof(value);
        else if (name == "boost_distance")
            _boostDistance = std::stof(value);
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        _startPitch = _pitch;
        if (const std::optional<Entity> rat = findRat(scene))
            _startHorizontalAngle = scene.getComponent<Transform>(*rat).rotationY;
        _horizontalAngle = _startHorizontalAngle;
        place(scene, entity);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (scene.getResource<GameState>().respawns != _respawns) {
            _respawns = scene.getResource<GameState>().respawns;
            _pitch = _startPitch;
            _horizontalAngle = _startHorizontalAngle;
        }
        if (scene.getResource<GameState>().canMove) {
            const MouseDelta mouse =
                scene.getResource<std::reference_wrapper<IRenderer>>().get().getMouseDelta();
            _horizontalAngle = std::fmod(_horizontalAngle + mouse.x * _sensitivity, 360.0f);
            _pitch += mouse.y * _sensitivity;
            _pitch = std::clamp(_pitch, RAT_CAMERA_MIN_PITCH, RAT_CAMERA_MAX_PITCH);
        }
        const float zoom = scene.getResource<GameState>().boosted ? _boostDistance : 1.0f;
        _zoom += (zoom - _zoom) * std::min(RAT_CAMERA_ZOOM_SPEED * FIXED_DELTA_TIME, 1.0f);
        place(scene, entity);
    }

  private:
    std::optional<Entity> findRat(Scene &scene) const
    {
        const std::optional<Entity> rat = scene.getResource<EditorEntityIds>().find(_ratId);
        if (!rat || !scene.hasComponent<Transform>(*rat))
            return std::nullopt;
        return rat;
    }

    void place(Scene &scene, Entity entity) const
    {
        const std::optional<Entity> rat = findRat(scene);
        if (!rat)
            return;
        const Transform &target = scene.getComponent<Transform>(*rat);
        const float horizontal = _horizontalAngle * RAT_CAMERA_DEGREES_TO_RADIANS;
        const float pitch = _pitch * RAT_CAMERA_DEGREES_TO_RADIANS;
        const float distance = _distance * _zoom;
        const float flat = std::cos(pitch) * distance;
        Transform &position = scene.getComponent<Transform>(entity);
        Camera &camera = scene.getComponent<Camera>(entity);

        position.x = target.x - std::sin(horizontal) * flat;
        position.y = target.y + _targetHeight + std::sin(pitch) * distance;
        position.z = target.z + std::cos(horizontal) * flat;
        camera.targetX = target.x;
        camera.targetY = target.y + _targetHeight;
        camera.targetZ = target.z;
    }

    int _ratId = -1;
    float _distance = 17.0f;
    float _pitch = 20.0f;
    float _targetHeight = 1.0f;
    float _sensitivity = 0.1f;
    float _horizontalAngle = 0.0f;
    float _boostDistance = 1.3f;
    float _zoom = 1.0f;
    float _startPitch = 20.0f;
    float _startHorizontalAngle = 0.0f;
    int _respawns = 0;
};

#endif /* !RATCAMERA_HPP_ */
