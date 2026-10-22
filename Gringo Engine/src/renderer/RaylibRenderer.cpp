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
    SetTargetFPS(60);
    _font = LoadFontEx("Assets/Font.otf", 60, nullptr, 0);
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


Position RaylibRenderer::getTextSize(const std::string& text, int fontSize)
{
    Vector2 textSize = MeasureTextEx(_font, text.c_str(), static_cast<float>(fontSize), 0);

    return {textSize.x, textSize.y};
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