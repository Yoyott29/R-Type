/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Model3DSystem
*/

#include "Model3DSystem.hpp"
#include <functional>

void Model3DSystem::update(Scene &scene)
{
    if (!scene.hasResource<std::reference_wrapper<IRenderer>>())
        return;
    IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();
    const RendererCamera3D &camera = scene.getResource<ActiveCamera3D>().value;
    for (Entity entity : _entities) {
        const Transform &position = scene.getComponent<Transform>(entity);
        const Model3D &model = scene.getComponent<Model3D>(entity);
        if (model.primitive == "Model" && model.path.empty())
            continue;
        renderer.drawModel3D(camera,
                             {model.path, model.primitive, model.texturePath, model.color,
                              model.width, model.height, model.depth, model.scale,
                              position.scaleX, position.scaleY, position.scaleZ,
                              position.rotationX, position.rotationY, position.rotationZ,
                              model.animation, model.animationSpeed, model.animationFrame},
                             position.x, position.y, position.z);
    }
}
