/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** CameraSystem
*/

#include "CameraSystem.hpp"

void CameraSystem::update(Scene &scene)
{
    for (Entity entity : _entities) {
        const Camera &camera = scene.getComponent<Camera>(entity);
        if (!camera.active)
            continue;
        const Transform &position = scene.getComponent<Transform>(entity);
        scene.getResource<ActiveCamera3D>().value = {position.x, position.y, position.z,
                                                     camera.targetX, camera.targetY, camera.targetZ,
                                                     camera.upX, camera.upY, camera.upZ,
                                                     camera.fieldOfView};
        break;
    }
}
