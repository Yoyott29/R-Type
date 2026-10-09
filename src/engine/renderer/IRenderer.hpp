/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** IRenderer
*/

#pragma once
#include "ColorRGBA.hpp"
#include <string>

class IRenderer
{
  public:
    virtual ~IRenderer() = default;
    virtual void init(int width, int height, std::string const &title) = 0;
    virtual bool shouldClose() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear(ColorRGBA color) = 0;
    virtual void drawRectangle(float x, float y, float width, float height, ColorRGBA color) = 0;
    virtual float getFrameTime() = 0;
};
