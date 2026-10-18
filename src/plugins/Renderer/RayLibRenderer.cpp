/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RayLibRenderer
*/

#include "RayLibRenderer.hpp"
#include "EngineError.hpp"
#include <functional>
#include <variant>

RayLibRenderer::~RayLibRenderer()
{
    if (IsWindowReady())
        CloseWindow();
}

void RayLibRenderer::build(Application &app)
{
    app._scene.addResource<std::reference_wrapper<IRenderer>>(*this);

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
    if (!IsWindowReady())
        throw EngineError("Could not open the window");
    SetTargetFPS(60);
}

void RayLibRenderer::cleanup(Application &app)
{
    (void)app;
 //   Cleanup logic for the RayLibRenderer plugin
}

void RayLibRenderer::beginFrame()
{
    BeginDrawing();
}

void RayLibRenderer::clearBackground()
{
    ClearBackground({0, 0, 0, 255});
}

void RayLibRenderer::drawSprite(Position const &position, Sprite const &sprite)
{
    if (std::holds_alternative<BoxShape>(sprite.shape)) {
        BoxShape const &box = std::get<BoxShape>(sprite.shape);
        DrawRectangleV({position.x - box.width / 2.0f, position.y - box.height / 2.0f},
                       {box.width, box.height},
                       {sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a});
        return;
    }

    CircleShape const &circle = std::get<CircleShape>(sprite.shape);
    DrawCircleV({position.x, position.y}, circle.radius,
                {sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a});
}

void RayLibRenderer::endFrame()
{
    EndDrawing();
}