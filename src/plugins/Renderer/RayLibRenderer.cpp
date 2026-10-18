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
    unloadTextures();
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

    // raylib inits
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

void RayLibRenderer::beginFrame() { BeginDrawing(); }

void RayLibRenderer::clearBackground() { ClearBackground({0, 0, 0, 255}); }

void RayLibRenderer::drawSprite(Position const &position, Sprite const &sprite)
{
    Vector2 size;
    if (std::holds_alternative<BoxShape>(sprite.shape)) {
        BoxShape const &box = std::get<BoxShape>(sprite.shape);
        size = {box.width, box.height};
    } else {
        CircleShape const &circle = std::get<CircleShape>(sprite.shape);
        size = {circle.radius * 2.0f, circle.radius * 2.0f};
    }

    if (!sprite.texturePath.empty()) {
        Texture2D &texture = getTexture(sprite.texturePath);
        Rectangle source = {0.0f, 0.0f, static_cast<float>(texture.width),
                            static_cast<float>(texture.height)};
        Rectangle destination = {position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x,
                                 size.y};
        DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
        return;
    }

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

void RayLibRenderer::endFrame() { EndDrawing(); }

Texture2D &RayLibRenderer::getTexture(std::string const &path)
{
    auto [iterator, inserted] = _textures.try_emplace(path);
    if (inserted) {
        iterator->second = LoadTexture(path.c_str());
        if (iterator->second.id == 0) {
            _textures.erase(iterator);
            throw FileNotFound(path);
        }
    }
    return iterator->second;
}

void RayLibRenderer::unloadTextures()
{
    for (auto &[path, texture] : _textures) {
        (void)path;
        if (texture.id != 0)
            UnloadTexture(texture);
    }
    _textures.clear();
}
