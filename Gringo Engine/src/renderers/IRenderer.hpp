/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** IRenderer
*/

#pragma once
#include "ColorRGBA.hpp"
#include "keyboard.hpp"
#include "PluginLoader.hpp"
#include <memory>
#include <string>

struct RendererCamera3D {
    float positionX;
    float positionY;
    float positionZ;
    float targetX;
    float targetY;
    float targetZ;
    float upX;
    float upY;
    float upZ;
    float fieldOfView;
};

struct RendererModel3D {
    std::string path;
    std::string primitive = "Model";
    float scale = 1.0f;
};

// The most basic functions a renderer gives: a window, drawing simple things and the keyboard.
// A renderer (src/renderers/<Name>) derives from Renderer<T> below and implements this; plugins
// (2D, UI, InputHandler...) build their features by calling it. The application owns the renderer the project chose, and puts it
// in the scene as std::reference_wrapper<IRenderer>.
// The game draws in a fixed world (openWindow's width x height), scaled to the window. Every
// (x, y) given to a draw function is the CENTER of what is drawn
class IRenderer
{
  public:
    virtual ~IRenderer() = default;

    // Window
    virtual void openWindow(int width, int height, std::string const &title) = 0;
    virtual void closeWindow() = 0;
    // False once the user asked to close the window
    virtual bool isWindowOpen() const = 0;

    // Frame: everything is drawn between beginFrame and endFrame, which shows it
    virtual void beginFrame(ColorRGBA background) = 0;
    virtual void endFrame() = 0;

    // Drawing
    virtual void drawRectangle(float x, float y, float width, float height, ColorRGBA color) = 0;
    virtual void drawCircle(float x, float y, float radius, ColorRGBA color) = 0;
    // fontPath: a .ttf / .otf file; empty or not loadable: the renderer's built-in font
    virtual void drawText(float x, float y, std::string const &text, int fontSize,
                          ColorRGBA color, std::string const &fontPath) = 0;
    // Draws an image file stretched to width x height
    virtual void drawImage(std::string const &path, float x, float y, float width,
                           float height) = 0;
    // Draws the part of an image file starting at (sourceX, sourceY), of size
    // sourceWidth x sourceHeight in the image's pixels, stretched to width x height. Used for the
    // frames of a spritesheet
    virtual void drawImageRegion(std::string const &path, float x, float y, float width,
                                 float height, float sourceX, float sourceY, float sourceWidth,
                                 float sourceHeight) = 0;

    virtual void drawModel3D(RendererCamera3D const &, RendererModel3D const &, float, float,
                             float) = 0;
    // Keyboard
    virtual bool isKeyDown(Key key) const = 0;

    // Mouse
    // Cursor position in world coordinates, so it can be compared with what is drawn
    virtual MousePosition getMousePosition() const = 0;
    // True only on the frame the button went down
    virtual bool isMouseButtonPressed(Mouse button) const = 0;
};

// Base class of every renderer: class RaylibRenderer : public Renderer<RaylibRenderer>. When the
// renderer's library is opened, it registers how to create it, so the engine can load it by name
template <typename Derived> class Renderer : public IRenderer
{
  public:
    // Uses _registered, so it exists (and runs) as soon as the renderer class is compiled
    ~Renderer() override { (void)_registered; }

  private:
    static std::shared_ptr<IRenderer> create() { return std::make_shared<Derived>(); }
    static inline const bool _registered = PluginLoader::registerRenderer(&Renderer::create);
};
