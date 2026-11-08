/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RaylibRenderer
*/

#ifndef RAYLIBRENDERER_HPP_
#define RAYLIBRENDERER_HPP_

#include "IEditorRenderer.hpp"
#include <map>
#include <raylib.h>
#include <string>

class RaylibRenderer : public IEditorRenderer
{
  public:
    ~RaylibRenderer() override;

    // Initialization and cleanup
    void initWindow(int width, int height, const std::string &title) override;
    void clearScreen(ColorRGBA color) override;
    void startDrawing() override;
    void endDrawing() override;
    bool shouldClose() const override;
    void close() override;

    // Getters
    Vec2 getTextSize(const std::string &text, int fontSize) override;
    Vec2 getMousePosition() const override;
    bool isMouseButtonPressed(MouseButtons button) const override;
    bool isMouseButtonDown(MouseButtons button) const override;
    bool isKeyPressed(KeyboardKeys key) const override;
    int getNextCharacter() const override;

    // Drawing functions
    void drawRectangle(Vec2 pos, int width, int height, ColorRGBA color) override;
    void drawText(const std::string &text, Vec2 pos, int fontSize, ColorRGBA color) override;
    void drawImage(const std::string &file, Vec2 pos, int width, int height) override;

  private:
    // A file of the editor's Assets folder (src/editor/Assets), found from the editor's own folder,
    // so the editor can be started from any folder
    static std::string assetPath(const std::string &file);

    Font _font{};
    // Loaded the first time they are drawn, freed when the window closes
    std::map<std::string, Texture2D> _textures;
    bool _closed = false;
};

#endif /* !RAYLIBRENDERER_HPP_ */
