/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Game renderer interface
*/

#pragma once
#include "ColorRGBA.hpp"
#include "Position.hpp"
#include "Sprite.hpp"
#include <string>

class IRenderer
{
  public:
    virtual ~IRenderer() = default;

    virtual void beginFrame() = 0;
    virtual void clearBackground() = 0;
    virtual void drawSprite(Position const &position, Sprite const &sprite) = 0;
    // (x, y) is the top-left corner of the text, in world coordinates
    virtual void drawText(float x, float y, std::string const &text, int fontSize,
                          ColorRGBA color) = 0;
    virtual void endFrame() = 0;
    virtual void playSound(std::string const &path) = 0;
    virtual void playMusic(std::string const &path) = 0;
    virtual void stopMusic() = 0;
};
