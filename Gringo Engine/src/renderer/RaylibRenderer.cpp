/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RaylibRenderer
*/

#include "RaylibRenderer.hpp"

RaylibRenderer::~RaylibRenderer()
{
    UnloadFont(_font);
    CloseWindow();
}

void RaylibRenderer::initWindow(int width, int height, const std::string& title)
{
    SetTraceLogLevel(LOG_NONE);
    InitWindow(width, height, title.c_str());
    ToggleFullscreen();
    SetTargetFPS(60);
    _font = LoadFontEx("Assets/Font.ttf", 60, nullptr, 0);
}


void RaylibRenderer::clearScreen(ColorRGBA color)
{
    ClearBackground({color.r, color.g, color.b, color.a});
}

void RaylibRenderer::startDrawing()
{
    BeginDrawing();
}

void RaylibRenderer::endDrawing()
{
    EndDrawing();
}

bool RaylibRenderer::shouldClose() const
{
    return WindowShouldClose();
}

Position RaylibRenderer::getTextSize(const std::string& text, int fontSize)
{
    Vector2 textSize = MeasureTextEx(_font, text.c_str(), static_cast<float>(fontSize), 0);

    return {textSize.x, textSize.y};
}

Position RaylibRenderer::getMousePosition() const
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

int RaylibRenderer::getNextCharacter() const
{
    return GetCharPressed();
}

void RaylibRenderer::drawRectangle(Position pos, int width, int height, ColorRGBA color)
{
    DrawRectangle(static_cast<int>(pos.x - width / 2.0f),
                  static_cast<int>(pos.y - height / 2.0f),
                  width, height, {color.r, color.g, color.b, color.a});
}

void RaylibRenderer::drawText(const std::string& text, Position pos, int fontSize,
                              ColorRGBA color)
{
    Position size = getTextSize(text, fontSize);
    Vector2 topLeft = {pos.x - size.x / 2.0f, pos.y - size.y / 2.0f};

    DrawTextEx(_font, text.c_str(), topLeft, static_cast<float>(fontSize), 0,
               {color.r, color.g, color.b, color.a});
}