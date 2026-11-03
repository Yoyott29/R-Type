/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RaylibRenderer
*/

#include "RaylibRenderer.hpp"
#include "EngineError.hpp"
#include <algorithm>

RaylibRenderer::~RaylibRenderer() { closeWindow(); }

void RaylibRenderer::openWindow(int width, int height, std::string const &title)
{
    _worldWidth = static_cast<float>(width);
    _worldHeight = static_cast<float>(height);
    SetTraceLogLevel(LOG_NONE);
    InitWindow(width, height, title.c_str());
    if (!IsWindowReady())
        throw EngineError("Could not open the window");
    SetTargetFPS(60);
}

void RaylibRenderer::closeWindow()
{
    // Textures belong to the window's graphics context: freed before it closes
    unloadTextures();
    if (IsWindowReady())
        CloseWindow();
}

bool RaylibRenderer::isWindowOpen() const { return IsWindowReady() && !WindowShouldClose(); }

void RaylibRenderer::beginFrame(ColorRGBA background)
{
    // The game draws in a fixed world size, scaled and centered in the window whatever its size
    const float zoom = std::min(static_cast<float>(GetScreenWidth()) / _worldWidth,
                                static_cast<float>(GetScreenHeight()) / _worldHeight);
    const Vector2 offset = {(static_cast<float>(GetScreenWidth()) - _worldWidth * zoom) / 2.0f,
                            (static_cast<float>(GetScreenHeight()) - _worldHeight * zoom) / 2.0f};
    BeginDrawing();
    ClearBackground(toRaylib(background));
    BeginMode2D({offset, {0.0f, 0.0f}, 0.0f, zoom});
}

void RaylibRenderer::endFrame()
{
    EndMode2D();
    EndDrawing();
}

void RaylibRenderer::drawRectangle(float x, float y, float width, float height, ColorRGBA color)
{
    DrawRectangleV({x - width / 2.0f, y - height / 2.0f}, {width, height}, toRaylib(color));
}

void RaylibRenderer::drawCircle(float x, float y, float radius, ColorRGBA color)
{
    DrawCircleV({x, y}, radius, toRaylib(color));
}

void RaylibRenderer::drawText(float x, float y, std::string const &text, int fontSize,
                              ColorRGBA color)
{
    // Same font and spacing as raylib's DrawText, measured so (x, y) is the center of the text
    const Font font = GetFontDefault();
    const float size = static_cast<float>(std::max(fontSize, 10));
    const float spacing = size / 10.0f;
    const Vector2 bounds = MeasureTextEx(font, text.c_str(), size, spacing);
    DrawTextEx(font, text.c_str(), {x - bounds.x / 2.0f, y - bounds.y / 2.0f}, size, spacing,
               toRaylib(color));
}

void RaylibRenderer::drawImage(std::string const &path, float x, float y, float width,
                               float height)
{
    Texture2D &texture = getTexture(path);
    const Rectangle source = {0.0f, 0.0f, static_cast<float>(texture.width),
                              static_cast<float>(texture.height)};
    const Rectangle destination = {x - width / 2.0f, y - height / 2.0f, width, height};
    DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
}

bool RaylibRenderer::isKeyDown(Key key) const
{
    return key != Key::Unknown && IsKeyDown(toRaylib(key));
}

Color RaylibRenderer::toRaylib(ColorRGBA color) { return {color.r, color.g, color.b, color.a}; }

KeyboardKey RaylibRenderer::toRaylib(Key key)
{
    switch (key) {
        case Key::Z: return KEY_Z;
        case Key::S: return KEY_S;
        case Key::Q: return KEY_Q;
        case Key::D: return KEY_D;
        case Key::Up: return KEY_UP;
        case Key::Down: return KEY_DOWN;
        case Key::Left: return KEY_LEFT;
        case Key::Right: return KEY_RIGHT;
        case Key::Space: return KEY_SPACE;
        case Key::Unknown: break;
    }
    return KEY_NULL;
}

Texture2D &RaylibRenderer::getTexture(std::string const &path)
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

void RaylibRenderer::unloadTextures()
{
    for (auto &[path, texture] : _textures) {
        (void)path;
        if (texture.id != 0)
            UnloadTexture(texture);
    }
    _textures.clear();
}
