/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Renderer interface
*/

#pragma once
#include "Structs.hpp"
#include <string>

class IRenderer
{
  public:
      virtual ~IRenderer() = default;

      // Initialization and cleanup
      virtual void initWindow(int width, int height, const std::string& title) = 0;
      virtual void clearScreen(ColorRGBA color) = 0;
      virtual void startDrawing() = 0;
      virtual void endDrawing() = 0;

      // Getters
      virtual Position getTextSize(const std::string& text, int fontSize) = 0;

      // Drawing functions
      virtual void drawRectangle(Position pos, int width, int height, ColorRGBA color) = 0;
      virtual void drawText(const std::string& text, Position pos, int fontSize, ColorRGBA color) = 0;
};
