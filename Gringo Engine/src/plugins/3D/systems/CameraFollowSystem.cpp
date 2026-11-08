#include "CameraFollowSystem.hpp"
#include <cmath>

void CameraFollowSystem::update(Scene &scene)
{
    constexpr float degreesToRadians = 3.14159265358979323846f / 180.0f;

    for (Entity cameraEntity : _entities) {
        const ChildOf &childOf = scene.getComponent<ChildOf>(cameraEntity);
        if (childOf.parent == NO_PARENT || !scene.isEntityAlive(childOf.parent) ||
            !scene.hasComponent<Transform>(childOf.parent))
            continue;

        const Transform &target = scene.getComponent<Transform>(childOf.parent);
        Transform &cameraTransform = scene.getComponent<Transform>(cameraEntity);
        Camera &camera = scene.getComponent<Camera>(cameraEntity);
        const CameraFollow3D &follow = scene.getComponent<CameraFollow3D>(cameraEntity);
        const float yaw = target.rotationY * degreesToRadians;

        // The target faces along its local -Z: the camera stays behind it, on its local +Z
        cameraTransform.x = target.x - std::sin(yaw) * follow.distance;
        cameraTransform.y = target.y + follow.height;
        cameraTransform.z = target.z + std::cos(yaw) * follow.distance;
        camera.targetX = target.x;
        camera.targetY = target.y + follow.targetHeight;
        camera.targetZ = target.z;
    }
}
