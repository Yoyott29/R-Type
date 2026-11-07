#ifndef RATCAMERA_HPP_
#define RATCAMERA_HPP_

#include "Camera.hpp"
#include "GameState.hpp"
#include "IRenderer.hpp"
#include "ProjectComponentManager.hpp"
#include "Scene.hpp"
#include "Script.hpp"
#include "Transform.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <vector>

// Lowest and highest the camera can look down at the rat from, in degrees
#define RAT_CAMERA_MIN_PITCH 5.0f
#define RAT_CAMERA_MAX_PITCH 80.0f

// "rat": editor id of the entity followed. Moving the mouse sideways turns the camera around it,
// up and down raises and lowers it; it does not turn with the rat
inline const std::vector<std::string> RatCamera_params = {"rat", "distance", "pitch",
                                                          "target_height", "sensitivity"};

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
    }

    void onStart(Scene &scene, Entity &entity) override
    {
        if (!scene.hasResource<GameState>())
            scene.addResource<GameState>();
        // Starts behind the rat
        if (const std::optional<Entity> rat = findRat(scene))
            _yaw = scene.getComponent<Transform>(*rat).rotationY;
        place(scene, entity);
    }

    void onUpdate(Scene &scene, Entity &entity) override
    {
        if (scene.getResource<GameState>().canMove) {
            const MouseDelta mouse =
                scene.getResource<std::reference_wrapper<IRenderer>>().get().getMouseDelta();
            _yaw = std::fmod(_yaw + mouse.x * _sensitivity, 360.0f);
            // Mouse down: the camera goes up and looks down more
            _pitch += mouse.y * _sensitivity;
            _pitch = std::clamp(_pitch, RAT_CAMERA_MIN_PITCH, RAT_CAMERA_MAX_PITCH);
        }
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

    // Behind the rat as seen along _yaw (looking along (sin, -cos) of it), _pitch above the floor
    void place(Scene &scene, Entity entity) const
    {
        const std::optional<Entity> rat = findRat(scene);
        if (!rat)
            return;
        const Transform &target = scene.getComponent<Transform>(*rat);
        const float yaw = _yaw * 3.14159265358979323846f / 180.0f;
        const float pitch = _pitch * 3.14159265358979323846f / 180.0f;
        const float flat = std::cos(pitch) * _distance;
        Transform &position = scene.getComponent<Transform>(entity);
        Camera &camera = scene.getComponent<Camera>(entity);

        position.x = target.x - std::sin(yaw) * flat;
        position.y = target.y + _targetHeight + std::sin(pitch) * _distance;
        position.z = target.z + std::cos(yaw) * flat;
        camera.targetX = target.x;
        camera.targetY = target.y + _targetHeight;
        camera.targetZ = target.z;
    }

    int _ratId = -1;
    float _distance = 17.0f;
    float _pitch = 20.0f;
    float _targetHeight = 1.0f;
    float _sensitivity = 0.1f;
    float _yaw = 0.0f;
};

#endif /* !RATCAMERA_HPP_ */
