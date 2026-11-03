/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RayLibRenderer
*/

#include "RayLibRenderer.hpp"
#include "EngineError.hpp"
#include "PluginApi.hpp"
#include "ComponentJson.hpp"
#include "ProjectAssets.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <variant>

RayLibRenderer::~RayLibRenderer()
{
    unloadTextures();
    unloadSounds();
    unloadMusic();
    if (IsAudioDeviceReady())
        CloseAudioDevice();
    if (IsWindowReady())
        CloseWindow();
}

void RayLibRenderer::registerComponents(Scene &scene)
{
    // register components, with how to read them from a .gringo file
    scene.registerComponent<Sprite>([](Scene &target, Entity entity, const nlohmann::json &data) {
        Sprite sprite;
        if (ComponentJson::value(data, "Shape", "Box") == "Circle")
            sprite.shape = CircleShape{ComponentJson::floatValue(data, "Radius", 16.0f)};
        else
            sprite.shape = BoxShape{ComponentJson::floatValue(data, "Width", 32.0f),
                                    ComponentJson::floatValue(data, "Height", 32.0f)};
        sprite.drawOrder = ComponentJson::intValue(data, "DrawOrder", 0);
        const std::string texture = ComponentJson::value(data, "TexturePath", "");
        sprite.texturePath = texture == "default.png" ? "" : ProjectAssets::resolve(target, texture);
        sprite.color = ComponentJson::colorValue(data, "Color", {255, 255, 255, 255});
        target.addComponent(entity, sprite);
    });
    scene.registerComponent<SoundEffect>(
        [](Scene &target, Entity entity, const nlohmann::json &data) {
            target.addComponent(
                entity,
                SoundEffect{ProjectAssets::resolve(
                                target, ComponentJson::value(data, "Path", "sound.wav")),
                            ComponentJson::boolValue(data, "Play", false)});
        });
}

void RayLibRenderer::build(Application &app)
{
    app._scene.addResource<std::reference_wrapper<IRenderer>>(*this);

    // Register the GetInput system and add the KeyboardInput resource
    app._scene.addResource<KeyboardInput>();
    app._scene.registerSystem<GetInput>(PrepUpdate);

    app._scene.registerSystem<SoundSystem>(PostUpdate);
    ComponentSet soundRequirements;
    soundRequirements.set(app._scene.getComponentType<SoundEffect>());
    app._scene.setSystemComponentSet<SoundSystem>(soundRequirements);

    // Register the render system
    auto renderSystem = app._scene.registerSystem<RenderSystem>(PostUpdate);
    ComponentSet renderRequirements;
    renderRequirements.set(app._scene.getComponentType<Position>());
    renderRequirements.set(app._scene.getComponentType<Sprite>());
    app._scene.setSystemComponentSet<RenderSystem>(renderRequirements);

    // Ends the frame after the Overlay systems (UI) drew on top of the world
    app._scene.registerSystem<DisplaySystem>(Display);

    // raylib inits
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WORLD_WIDTH, WORLD_HEIGHT, WINDOW_TITLE);
    if (!IsWindowReady())
        throw EngineError("Could not open the window");
    InitAudioDevice();
    SetTargetFPS(60);
}

void RayLibRenderer::cleanup(Application &app) { (void)app; }

void RayLibRenderer::beginFrame()
{
    if (_music)
        UpdateMusicStream(*_music);
    // The game always draws in WORLD_WIDTH x WORLD_HEIGHT coordinates, whatever the window size
    float const zoom = std::min(static_cast<float>(GetScreenWidth()) / WORLD_WIDTH,
                                static_cast<float>(GetScreenHeight()) / WORLD_HEIGHT);
    Vector2 const offset = {(static_cast<float>(GetScreenWidth()) - WORLD_WIDTH * zoom) / 2.0f,
                            (static_cast<float>(GetScreenHeight()) - WORLD_HEIGHT * zoom) / 2.0f};
    BeginDrawing();
    BeginMode2D({offset, {0.0f, 0.0f}, 0.0f, zoom});
}

void RayLibRenderer::clearBackground() { ClearBackground({0, 0, 0, 255}); }

void RayLibRenderer::drawSprite(Position const &position, Sprite const &sprite)
{
    Vector2 size;
    if (std::holds_alternative<BoxShape>(sprite.shape)) {
        BoxShape const &box = std::get<BoxShape>(sprite.shape);
        size = {box.width, box.height};
    } else {
        CircleShape const &circle = std::get<CircleShape>(sprite.shape);
        size = {circle.radius * 2.0f, circle.radius * 2.0f};
    }

    if (!sprite.texturePath.empty()) {
        Texture2D &texture = getTexture(sprite.texturePath);
        Rectangle source = {0.0f, 0.0f, static_cast<float>(texture.width),
                            static_cast<float>(texture.height)};
        Rectangle destination = {position.x - size.x / 2.0f, position.y - size.y / 2.0f, size.x,
                                 size.y};
        DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
        return;
    }

    if (std::holds_alternative<BoxShape>(sprite.shape)) {
        BoxShape const &box = std::get<BoxShape>(sprite.shape);
        DrawRectangleV({position.x - box.width / 2.0f, position.y - box.height / 2.0f},
                       {box.width, box.height},
                       {sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a});
        return;
    }

    CircleShape const &circle = std::get<CircleShape>(sprite.shape);
    DrawCircleV({position.x, position.y}, circle.radius,
                {sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a});
}

void RayLibRenderer::drawText(float x, float y, std::string const &text, int fontSize,
                              ColorRGBA color)
{
    DrawText(text.c_str(), static_cast<int>(x), static_cast<int>(y), fontSize,
             {color.r, color.g, color.b, color.a});
}

void RayLibRenderer::endFrame()
{
    EndMode2D();
    EndDrawing();
}

Texture2D &RayLibRenderer::getTexture(std::string const &path)
{
    auto [iterator, inserted] = _textures.try_emplace(path);
    if (inserted) {
        iterator->second = LoadTexture(path.c_str());
        if (iterator->second.id == 0) {
            _textures.erase(iterator);
            throw FileNotFound(path);
        }
    }
    return iterator->second;
}

void RayLibRenderer::unloadTextures()
{
    for (auto &[path, texture] : _textures) {
        (void)path;
        if (texture.id != 0)
            UnloadTexture(texture);
    }
    _textures.clear();
}

void RayLibRenderer::playSound(std::string const &path)
{
    if (!IsAudioDeviceReady())
        return;
    PlaySound(getSound(path));
}

Sound &RayLibRenderer::getSound(std::string const &path)
{
    auto [iterator, inserted] = _sounds.try_emplace(path);
    if (inserted) {
        iterator->second = LoadSound(path.c_str());
        if (!IsSoundValid(iterator->second)) {
            _sounds.erase(iterator);
            throw FileNotFound(path);
        }
    }
    return iterator->second;
}

void RayLibRenderer::unloadSounds()
{
    for (auto &[path, sound] : _sounds) {
        (void)path;
        UnloadSound(sound);
    }
    _sounds.clear();
}

void RayLibRenderer::playMusic(std::string const &path)
{
    if (!IsAudioDeviceReady())
        return;
    unloadMusic();

    Music music = LoadMusicStream(path.c_str());
    if (!IsMusicValid(music)) {
        std::cerr << "Could not load the music " << path << '\n';
        return;
    }
    _music = music;
    PlayMusicStream(*_music);
}

void RayLibRenderer::stopMusic() { unloadMusic(); }

void RayLibRenderer::unloadMusic()
{
    if (!_music)
        return;
    StopMusicStream(*_music);
    UnloadMusicStream(*_music);
    _music.reset();
}
PLUGIN_EXPORT IPlugin *createPlugin() { return new RayLibRenderer(); }
