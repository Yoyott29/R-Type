/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RaylibRenderer
*/

#ifndef RAYLIBRENDERER_HPP_
#define RAYLIBRENDERER_HPP_

#include "IEditorRenderer.hpp"
#include <raylib.h>

class RaylibRenderer : public IEditorRenderer {
    public:

    ~RaylibRenderer() override;

    // Initialization and cleanup
    void initWindow(int width, int height, const std::string& title) override;
    void clearScreen(ColorRGBA color) override;
    void startDrawing() override;
    void endDrawing() override;
    bool shouldClose() const override;
    void close() override;


    // Getters
    Vec2 getTextSize(const std::string& text, int fontSize) override;
    Vec2 getMousePosition() const override;
    bool isMouseButtonPressed(MouseButtons button) const override;
    bool isMouseButtonDown(MouseButtons button) const override;
    bool isKeyPressed(KeyboardKeys key) const override;
    int getNextCharacter() const override;

    // Drawing functions
    void drawRectangle(Vec2 pos, int width, int height, ColorRGBA color) override;
    void drawText(const std::string& text, Vec2 pos, int fontSize, ColorRGBA color) override;

    private:

    Font _font{};
    bool _closed = false;
};

#endif /* !RAYLIBRENDERER_HPP_ */
