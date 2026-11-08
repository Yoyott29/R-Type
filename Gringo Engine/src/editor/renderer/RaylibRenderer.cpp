/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RaylibRenderer
*/

#include "RaylibRenderer.hpp"

RaylibRenderer::~RaylibRenderer() { RaylibRenderer::close(); }

void RaylibRenderer::initWindow(int width, int height, const std::string &title)
{
    SetExitKey(KEY_NULL);
    SetTraceLogLevel(LOG_NONE);
    InitWindow(width, height, title.c_str());
    // The layout uses fixed pixels, so the size is kept: a window the window system placed
    // outside the screen (it can when the window is larger than the screen) is moved to its corner
    const int monitor = GetCurrentMonitor();
    const Vector2 origin = GetMonitorPosition(monitor);
    const Vector2 position = GetWindowPosition();
    if (position.x < origin.x - 100 || position.y < origin.y - 100 ||
        position.x > origin.x + static_cast<float>(GetMonitorWidth(monitor)) ||
        position.y > origin.y + static_cast<float>(GetMonitorHeight(monitor)))
        SetWindowPosition(static_cast<int>(origin.x), static_cast<int>(origin.y) + 40); // below the title bar
    SetTargetFPS(60);
    _font = LoadFontEx(assetPath("Font.ttf").c_str(), 60, nullptr, 0);
}

void RaylibRenderer::clearScreen(ColorRGBA color)
{
    ClearBackground({color.r, color.g, color.b, color.a});
}

void RaylibRenderer::startDrawing() { BeginDrawing(); }

void RaylibRenderer::endDrawing() { EndDrawing(); }

bool RaylibRenderer::shouldClose() const { return WindowShouldClose(); }

void RaylibRenderer::close()
{
    if (_closed)
        return;
    UnloadFont(_font);
    for (auto &entry : _textures)
        UnloadTexture(entry.second);
    _textures.clear();
    CloseWindow();
    _closed = true;
}

Vec2 RaylibRenderer::getTextSize(const std::string &text, int fontSize)
{
    Vector2 textSize = MeasureTextEx(_font, text.c_str(), static_cast<float>(fontSize), 0);

    return {textSize.x, textSize.y};
}

Vec2 RaylibRenderer::getMousePosition() const
{
    return {static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY())};
}

bool RaylibRenderer::isMouseButtonPressed(MouseButtons button) const
{
    return IsMouseButtonPressed(static_cast<int>(button));
}

bool RaylibRenderer::isMouseButtonDown(MouseButtons button) const
{
    return IsMouseButtonDown(static_cast<int>(button));
}

bool RaylibRenderer::isKeyPressed(KeyboardKeys key) const
{
    return IsKeyPressed(static_cast<int>(key));
}

int RaylibRenderer::getNextCharacter() const { return GetCharPressed(); }

void RaylibRenderer::drawRectangle(Vec2 pos, int width, int height, ColorRGBA color)
{
    DrawRectangle(static_cast<int>(pos.x - width / 2.0f), static_cast<int>(pos.y - height / 2.0f),
                  width, height, {color.r, color.g, color.b, color.a});
}

void RaylibRenderer::drawText(const std::string &text, Vec2 pos, int fontSize, ColorRGBA color)
{
    Vec2 size = getTextSize(text, fontSize);
    Vector2 topLeft = {pos.x - size.x / 2.0f, pos.y - size.y / 2.0f};

    DrawTextEx(_font, text.c_str(), topLeft, static_cast<float>(fontSize), 0,
               {color.r, color.g, color.b, color.a});
}

std::string RaylibRenderer::assetPath(const std::string &file)
{
    // GetApplicationDirectory ends with a separator
    return std::string(GetApplicationDirectory()) + "src/editor/Assets/" + file;
}

void RaylibRenderer::drawImage(const std::string &file, Vec2 pos, int width, int height)
{
    if (_textures.count(file) == 0) {
        Texture2D texture = LoadTexture(assetPath(file).c_str());
        // Nearest pixel: pixel art stays sharp when it is drawn bigger
        SetTextureFilter(texture, TEXTURE_FILTER_POINT);
        _textures[file] = texture;
    }
    Texture2D &texture = _textures[file];
    // A missing file gives an empty texture: nothing is drawn
    if (texture.id == 0)
        return;
    Rectangle source = {0.0f, 0.0f, static_cast<float>(texture.width),
                        static_cast<float>(texture.height)};
    Rectangle destination = {pos.x - width / 2.0f, pos.y - height / 2.0f, static_cast<float>(width),
                             static_cast<float>(height)};
    DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
}
