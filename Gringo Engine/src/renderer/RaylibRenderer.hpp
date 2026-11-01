/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RaylibRenderer
*/

#ifndef RAYLIBRENDERER_HPP_
#define RAYLIBRENDERER_HPP_

#include "IRenderer.hpp"
#include <raylib.h>

class RaylibRenderer : public IRenderer {
    public:

    ~RaylibRenderer() override;

    // Initialization and cleanup
    void initWindow(int width, int height, const std::string& title) override;
    void clearScreen(ColorRGBA color) override;
    void startDrawing() override;
    void endDrawing() override;
    bool shouldClose() const override;


    // Getters
    Position getTextSize(const std::string& text, int fontSize) override;
    Position getMousePosition() const override;
    bool isMouseButtonPressed(MouseButtons button) const override;
    bool isMouseButtonDown(MouseButtons button) const override;
    bool isKeyPressed(KeyboardKeys key) const override;
    int getNextCharacter() const override;

    // Drawing functions
    void drawRectangle(Position pos, int width, int height, ColorRGBA color) override;
    void drawText(const std::string& text, Position pos, int fontSize, ColorRGBA color) override;

    private:

    Font _font{};
};

#endif /* !RAYLIBRENDERER_HPP_ */
