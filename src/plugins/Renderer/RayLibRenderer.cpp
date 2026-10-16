/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RayLibRenderer
*/

#include "RayLibRenderer.hpp"

RayLibRenderer::~RayLibRenderer()
{
    if (IsWindowReady())
        CloseWindow();
}

void RayLibRenderer::build(Application &app)
{
    // register components
    app._scene.registerComponent<Sprite>();

    // Register the GetInput system and add the KeyboardInput resource
    app._scene.addResource<KeyboardInput>();
    app._scene.registerSystem<GetInput>(PrepUpdate);

    // Register the render system
    auto renderSystem = app._scene.registerSystem<RenderSystem>(PostUpdate);
    ComponentSet renderRequirements;
    renderRequirements.set(app._scene.getComponentType<Position>());
    renderRequirements.set(app._scene.getComponentType<Sprite>());
    app._scene.setSystemComponentSet<RenderSystem>(renderRequirements);

    //raylib inits
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);
}

void RayLibRenderer::cleanup(Application &app)
{
    (void)app;
 //   Cleanup logic for the RayLibRenderer plugin
}