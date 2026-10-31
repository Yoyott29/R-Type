/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RayLibRenderer
*/

#pragma once
#include "Application.hpp"
#include "GetInput.hpp"
#include "IRenderer.hpp"
#include "KeyboardInput.hpp"
#include "RenderSystem.hpp"
#include "Sprite.hpp"
#include <optional>
#include <raylib.h>
#include <string>
#include <unordered_map>
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "R-Type"

class RayLibRenderer : public IPlugin, public IRenderer
{
  public:
    RayLibRenderer() = default;
    ~RayLibRenderer();
    void build(Application &app) override;
    void cleanup(Application &app) override;

    void beginFrame() override;
    void clearBackground() override;
    void drawSprite(Position const &position, Sprite const &sprite) override;
    void endFrame() override;
    void playSound(std::string const &path) override;
    void playMusic(std::string const &path) override;
    void stopMusic() override;

  private:
    std::unordered_map<std::string, Texture2D> _textures;
    std::unordered_map<std::string, Sound> _sounds;
    std::optional<Music> _music;

    Texture2D &getTexture(std::string const &path);
    void unloadTextures();
    Sound &getSound(std::string const &path);
    void unloadSounds();
    void unloadMusic();
};
