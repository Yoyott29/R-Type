/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RaylibRenderer
*/

#pragma once
#include <raylib.h>
#include "IRenderer.hpp"

class RaylibRenderer : public IRenderer {
    public:
        ~RaylibRenderer() override;
        void init(int width, int height, std::string const &windowTitle) override;
        bool shouldClose() override;
        void beginFrame() override;
        void endFrame() override;
        void clear(ColorRGBA color) override;
        void drawRectangle(float x, float y, float width, float height, ColorRGBA color) override;
        float getFrameTime() override;

    private:
        Color getRaylibColor(ColorRGBA color);
};
