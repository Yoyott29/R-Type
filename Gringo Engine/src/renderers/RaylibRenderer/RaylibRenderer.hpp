/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RaylibRenderer
*/

#pragma once
#include "IRenderer.hpp"
#include <raylib.h>
#include <string>
#include <unordered_map>

// IRenderer implemented with raylib. Nothing else: features (sprites, UI...) come from plugins
class RaylibRenderer : public Renderer<RaylibRenderer>
{
  public:
    RaylibRenderer() = default;
    ~RaylibRenderer() override;

    void openWindow(int width, int height, std::string const &title) override;
    void closeWindow() override;
    bool isWindowOpen() const override;

    void beginFrame(ColorRGBA background) override;
    void endFrame() override;

    void drawRectangle(float x, float y, float width, float height, ColorRGBA color) override;
    void drawCircle(float x, float y, float radius, ColorRGBA color) override;
    void drawText(float x, float y, std::string const &text, int fontSize,
                  ColorRGBA color) override;
    void drawImage(std::string const &path, float x, float y, float width,
                   float height) override;

    bool isKeyDown(Key key) const override;

  private:
    static Color toRaylib(ColorRGBA color);
    static KeyboardKey toRaylib(Key key);
    // Loaded once, kept until the window closes
    Texture2D &getTexture(std::string const &path);
    void unloadTextures();

    float _worldWidth = 0.0f;
    float _worldHeight = 0.0f;
    std::unordered_map<std::string, Texture2D> _textures;
};
