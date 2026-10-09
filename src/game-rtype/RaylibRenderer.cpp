/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RaylibRenderer
*/

#include "RaylibRenderer.hpp"

RaylibRenderer::~RaylibRenderer()
{
    if (IsWindowReady())
        CloseWindow();
}

void RaylibRenderer::init(int width, int height, std::string const &windowTitle) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(width, height, windowTitle.c_str());
    SetTargetFPS(60);
}

bool RaylibRenderer::shouldClose()
{
    return WindowShouldClose();
}

void RaylibRenderer::beginFrame()
{
    BeginDrawing();
}

void RaylibRenderer::endFrame()
{
    EndDrawing();
}

void RaylibRenderer::clear(ColorRGBA color)
{
    ClearBackground({color.r, color.g, color.b, color.a});
}

void RaylibRenderer::drawRectangle(float x, float y, float width, float height, ColorRGBA color)
{
    DrawRectangle(
        static_cast<int>(x), static_cast<int>(y),
        static_cast<int>(width), static_cast<int>(height),
        getRaylibColor(color)
    );
}

float RaylibRenderer::getFrameTime()
{
    return GetFrameTime();
}

Color RaylibRenderer::getRaylibColor(ColorRGBA color)
{
    return Color{color.r, color.g, color.b, color.a};
}
