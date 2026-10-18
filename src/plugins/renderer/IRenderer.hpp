/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Renderer interface
*/

#pragma once
#include "Position.hpp"
#include "Sprite.hpp"

class IRenderer
{
  public:
    virtual ~IRenderer() = default;

    virtual void beginFrame() = 0;
    virtual void clearBackground() = 0;
    virtual void drawSprite(Position const &position, Sprite const &sprite) = 0;
    virtual void endFrame() = 0;
};
