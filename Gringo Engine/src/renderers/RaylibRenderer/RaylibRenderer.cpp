/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RaylibRenderer
*/

#include "RaylibRenderer.hpp"
#include "EngineError.hpp"
#include <algorithm>
#include <cmath>
#include <raymath.h>

// Size the fonts are loaded at: big enough to stay sharp when scaled to a text's size
const int FONT_LOAD_SIZE = 96;

const char *const COLOR_FILTER_SHADER = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform int mode;
out vec4 finalColor;

const mat3 SIMULATIONS[3] = mat3[3](
    mat3(vec3(0.152286, 1.052583, -0.204868), vec3(0.114503, 0.786281, 0.099216),
         vec3(-0.003882, -0.048116, 1.051998)),
    mat3(vec3(0.367322, 0.860646, -0.227968), vec3(0.280085, 0.672501, 0.047413),
         vec3(-0.011820, 0.042940, 0.968881)),
    mat3(vec3(1.255528, -0.076749, -0.178779), vec3(-0.078411, 0.930809, 0.147602),
         vec3(0.004733, 0.691367, 0.303900)));

const mat3 SHIFTS[3] = mat3[3](
    mat3(vec3(0.0, 0.0, 0.0), vec3(0.7, 1.0, 0.0), vec3(0.7, 0.0, 1.0)),
    mat3(vec3(0.0, 0.0, 0.0), vec3(0.7, 1.0, 0.0), vec3(0.7, 0.0, 1.0)),
    mat3(vec3(1.0, 0.0, 0.7), vec3(0.0, 1.0, 0.7), vec3(0.0, 0.0, 0.0)));

void main()
{
    vec4 color = texture(texture0, fragTexCoord) * colDiffuse * fragColor;
    vec3 error = color.rgb - color.rgb * SIMULATIONS[mode];
    finalColor = vec4(clamp(color.rgb + error * SHIFTS[mode], 0.0, 1.0), color.a);
}
)";

// Not closeWindow(): a virtual call in a destructor does not reach a derived class
RaylibRenderer::~RaylibRenderer() { releaseWindow(); }

void RaylibRenderer::openWindow(int width, int height, std::string const &title)
{
    _worldWidth = static_cast<float>(width);
    _worldHeight = static_cast<float>(height);
    SetTraceLogLevel(LOG_NONE);
    InitWindow(width, height, title.c_str());
    if (!IsWindowReady())
        throw EngineError("Could not open the window");
    SetTargetFPS(60);
}

void RaylibRenderer::closeWindow() { releaseWindow(); }

void RaylibRenderer::releaseWindow()
{
    // Textures and fonts belong to the window's graphics context: freed before it closes
    unloadTextures();
    unloadFonts();
    unloadColorFilter();

    if (IsWindowReady()) {
        EnableCursor();
        CloseWindow();
    }
}

bool RaylibRenderer::isWindowOpen() const { return IsWindowReady() && !WindowShouldClose(); }

Camera2D RaylibRenderer::worldCamera() const
{
    const float zoom = std::min(static_cast<float>(GetScreenWidth()) / _worldWidth,
                                static_cast<float>(GetScreenHeight()) / _worldHeight);
    const Vector2 offset = {(static_cast<float>(GetScreenWidth()) - _worldWidth * zoom) / 2.0f,
                            (static_cast<float>(GetScreenHeight()) - _worldHeight * zoom) / 2.0f};
    return {offset, {0.0f, 0.0f}, 0.0f, zoom};
}

void RaylibRenderer::setCursorLocked(bool locked) { _cursorLockRequested = locked; }

void RaylibRenderer::applyCursorLock()
{
    const bool requested = _cursorLockRequested;
    if (requested == _cursorLocked)
        return;
    _cursorLocked = requested;
    if (requested)
        DisableCursor();
    else
        EnableCursor();
}

void RaylibRenderer::beginFrame(ColorRGBA background)
{
    // Here because the window system only accepts it from the main thread
    applyCursorLock();
    BeginDrawing();
    if (_colorFilter != ColorFilter::None) {
        prepareFilterTarget();
        BeginTextureMode(_filterTarget);
    }
    ClearBackground(toRaylib(background));
    BeginMode2D(worldCamera());
}

void RaylibRenderer::endFrame()
{
    EndMode2D();
    if (_colorFilter != ColorFilter::None)
        drawFiltered();
    EndDrawing();
}

void RaylibRenderer::setColorFilter(ColorFilter filter)
{
    _colorFilter = filter;
    if (filter == ColorFilter::None || _filterShader.id != 0)
        return;
    _filterShader = LoadShaderFromMemory(nullptr, COLOR_FILTER_SHADER);
    _filterModeLocation = GetShaderLocation(_filterShader, "mode");
}

void RaylibRenderer::prepareFilterTarget()
{
    if (_filterTarget.id != 0 && _filterTarget.texture.width == GetScreenWidth() &&
        _filterTarget.texture.height == GetScreenHeight())
        return;
    if (_filterTarget.id != 0)
        UnloadRenderTexture(_filterTarget);
    _filterTarget = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
}

void RaylibRenderer::drawFiltered()
{
    EndTextureMode();
    const int mode = static_cast<int>(_colorFilter) - 1;
    const Rectangle source = {0.0f, 0.0f, static_cast<float>(_filterTarget.texture.width),
                              -static_cast<float>(_filterTarget.texture.height)};
    SetShaderValue(_filterShader, _filterModeLocation, &mode, SHADER_UNIFORM_INT);
    BeginShaderMode(_filterShader);
    DrawTextureRec(_filterTarget.texture, source, {0.0f, 0.0f}, WHITE);
    EndShaderMode();
}

void RaylibRenderer::unloadColorFilter()
{
    if (_filterTarget.id != 0)
        UnloadRenderTexture(_filterTarget);
    if (_filterShader.id != 0)
        UnloadShader(_filterShader);
    _filterTarget = RenderTexture2D{};
    _filterShader = Shader{};
}

void RaylibRenderer::drawRectangle(float x, float y, float width, float height, ColorRGBA color)
{
    DrawRectangleV({x - width / 2.0f, y - height / 2.0f}, {width, height}, toRaylib(color));
}

void RaylibRenderer::drawCircle(float x, float y, float radius, ColorRGBA color)
{
    DrawCircleV({x, y}, radius, toRaylib(color));
}

void RaylibRenderer::drawText(float x, float y, std::string const &text, int fontSize,
                              ColorRGBA color, std::string const &fontPath)
{
    // Same spacing as raylib's DrawText, measured so (x, y) is the center of the text
    const Font font = getFont(fontPath);
    const float size = static_cast<float>(std::max(fontSize, 10));
    const float spacing = size / 10.0f;
    const Vector2 bounds = MeasureTextEx(font, text.c_str(), size, spacing);
    DrawTextEx(font, text.c_str(), {x - bounds.x / 2.0f, y - bounds.y / 2.0f}, size, spacing,
               toRaylib(color));
}

void RaylibRenderer::drawImage(std::string const &path, float x, float y, float width, float height)
{
    Texture2D &texture = getTexture(path);
    const Rectangle source = {0.0f, 0.0f, static_cast<float>(texture.width),
                              static_cast<float>(texture.height)};
    const Rectangle destination = {x - width / 2.0f, y - height / 2.0f, width, height};
    DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
}

void RaylibRenderer::drawImageRegion(std::string const &path, float x, float y, float width,
                                     float height, float sourceX, float sourceY, float sourceWidth,
                                     float sourceHeight)
{
    Texture2D &texture = getTexture(path);
    const Rectangle source = {sourceX, sourceY, sourceWidth, sourceHeight};
    const Rectangle destination = {x - width / 2.0f, y - height / 2.0f, width, height};
    DrawTexturePro(texture, source, destination, {0.0f, 0.0f}, 0.0f, WHITE);
}

void RaylibRenderer::drawModel3D(RendererCamera3D const &camera, RendererModel3D const &model,
                                 float x, float y, float z)
{
    const ::Camera3D raylibCamera{{camera.positionX, camera.positionY, camera.positionZ},
                                  {camera.targetX, camera.targetY, camera.targetZ},
                                  {camera.upX, camera.upY, camera.upZ},
                                  camera.fieldOfView,
                                  CAMERA_PERSPECTIVE};
    EndMode2D();
    BeginMode3D(raylibCamera);
    if (model.texturePath.empty() && model.primitive == "Cube" && model.rotationX == 0.0f &&
        model.rotationY == 0.0f && model.rotationZ == 0.0f) {
        DrawCube({x, y, z}, model.width * model.scale * model.scaleX,
                 model.height * model.scale * model.scaleY,
                 model.depth * model.scale * model.scaleZ, toRaylib(model.color));
        EndMode3D();
        BeginMode2D(worldCamera());
        return;
    }
    if (model.texturePath.empty() && model.primitive == "Sphere" && model.rotationX == 0.0f &&
        model.rotationY == 0.0f && model.rotationZ == 0.0f && model.scaleX == model.scaleY &&
        model.scaleY == model.scaleZ) {
        DrawSphere({x, y, z}, model.scale * model.scaleX, toRaylib(model.color));
        EndMode3D();
        BeginMode2D(worldCamera());
        return;
    }

    const std::string modelKey =
        model.primitive == "Model"
            ? model.path
            : "__primitive_" + model.primitive + "_" + std::to_string(model.width) + "_" +
                  std::to_string(model.height) + "_" + std::to_string(model.depth);
    auto [iterator, inserted] = _models.try_emplace(modelKey);
    if (inserted) {
        if (model.primitive == "Cube")
            iterator->second =
                LoadModelFromMesh(GenMeshCube(model.width, model.height, model.depth));
        else if (model.primitive == "Sphere")
            iterator->second = LoadModelFromMesh(GenMeshSphere(0.5f, 32, 16));
        else
            iterator->second = LoadModel(model.path.c_str());
    }
    if (iterator->second.meshCount == 0)
        throw FileNotFound(model.path);

    if (!model.texturePath.empty()) {
        Texture2D &texture = getTexture(model.texturePath);
        for (int materialIndex = 0; materialIndex < iterator->second.materialCount;
             ++materialIndex) {
            iterator->second.materials[materialIndex].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
            iterator->second.materials[materialIndex].maps[MATERIAL_MAP_DIFFUSE].color =
                toRaylib(model.color);
        }
    }
    if (model.primitive == "Model" && model.animation >= 0) {
        AnimationSet &animationSet = getAnimations(model.path);
        if (model.animation >= animationSet.count)
            throw EngineError("Animation index out of range for " + model.path);
        ModelAnimation &animation = animationSet.animations[model.animation];
        // A held frame is only for this draw: the clock is shared by every model of this file, the
        // ones still playing go on from where they were
        float frame = 0.0f;
        if (model.animationFrame >= 0.0f) {
            frame = std::min(model.animationFrame, 1.0f) *
                    static_cast<float>(std::max(animation.keyframeCount - 1, 0));
        } else {
            animationSet.frame += GetFrameTime() * std::max(model.animationSpeed, 0.0f) *
                                  static_cast<float>(animation.keyframeCount);
            if (animation.keyframeCount > 0)
                animationSet.frame =
                    std::fmod(animationSet.frame, static_cast<float>(animation.keyframeCount));
            frame = animationSet.frame;
        }
        UpdateModelAnimation(iterator->second, animation, frame);
    }
    const float assetYaw = model.primitive == "Model" ? -90.0f : 0.0f;
    iterator->second.transform =
        MatrixMultiply(MatrixScale(model.scale * model.scaleX, model.scale * model.scaleY,
                                   model.scale * model.scaleZ),
                       MatrixMultiply(MatrixRotateX(model.rotationX * DEG2RAD),
                                      MatrixMultiply(MatrixRotateY(assetYaw * DEG2RAD),
                                                     MatrixRotateZ(model.rotationZ * DEG2RAD))));
    DrawModelEx(iterator->second, {x, y, z}, {0.0f, 1.0f, 0.0f}, -model.rotationY,
                {1.0f, 1.0f, 1.0f}, toRaylib(model.color));
    EndMode3D();
    BeginMode2D(worldCamera());
}

bool RaylibRenderer::isKeyDown(Key key) const
{
    return key != Key::Unknown && IsKeyDown(toRaylib(key));
}

MousePosition RaylibRenderer::getMousePosition() const
{
    // Same transform as the drawing, reversed: window pixels back to world coordinates
    const Vector2 world = GetScreenToWorld2D(GetMousePosition(), worldCamera());
    return {world.x, world.y};
}

MouseDelta RaylibRenderer::getMouseDelta() const
{
    const Vector2 delta = GetMouseDelta();
    return {delta.x, delta.y};
}

bool RaylibRenderer::isMouseButtonPressed(Mouse button) const
{
    return IsMouseButtonPressed(toRaylib(button));
}

Color RaylibRenderer::toRaylib(ColorRGBA color) { return {color.r, color.g, color.b, color.a}; }

KeyboardKey RaylibRenderer::toRaylib(Key key)
{
    switch (key) {
    // raylib names keys by their place on a QWERTY keyboard, the engine by their AZERTY label
    case Key::Z:
        return KEY_W;
    case Key::S:
        return KEY_S;
    case Key::Q:
        return KEY_A;
    case Key::D:
        return KEY_D;
    case Key::Up:
        return KEY_UP;
    case Key::Down:
        return KEY_DOWN;
    case Key::Left:
        return KEY_LEFT;
    case Key::Right:
        return KEY_RIGHT;
    case Key::Space:
        return KEY_SPACE;
    case Key::LeftShift:
        return KEY_LEFT_SHIFT;
    case Key::E:
        return KEY_E;
    case Key::A:
        return KEY_Q;
    case Key::B:
        return KEY_B;
    case Key::C:
        return KEY_C;
    case Key::F:
        return KEY_F;
    case Key::G:
        return KEY_G;
    case Key::H:
        return KEY_H;
    case Key::I:
        return KEY_I;
    case Key::J:
        return KEY_J;
    case Key::K:
        return KEY_K;
    case Key::L:
        return KEY_L;
    case Key::M:
        return KEY_SEMICOLON;
    case Key::N:
        return KEY_N;
    case Key::O:
        return KEY_O;
    case Key::P:
        return KEY_P;
    case Key::R:
        return KEY_R;
    case Key::T:
        return KEY_T;
    case Key::U:
        return KEY_U;
    case Key::V:
        return KEY_V;
    case Key::W:
        return KEY_Z;
    case Key::X:
        return KEY_X;
    case Key::Y:
        return KEY_Y;
    case Key::Num0:
        return KEY_ZERO;
    case Key::Num1:
        return KEY_ONE;
    case Key::Num2:
        return KEY_TWO;
    case Key::Num3:
        return KEY_THREE;
    case Key::Num4:
        return KEY_FOUR;
    case Key::Num5:
        return KEY_FIVE;
    case Key::Num6:
        return KEY_SIX;
    case Key::Num7:
        return KEY_SEVEN;
    case Key::Num8:
        return KEY_EIGHT;
    case Key::Num9:
        return KEY_NINE;
    case Key::Superscript2:
        return KEY_GRAVE;
    case Key::RightParenthesis:
        return KEY_MINUS;
    case Key::Equal:
        return KEY_EQUAL;
    case Key::Caret:
        return KEY_LEFT_BRACKET;
    case Key::Dollar:
        return KEY_RIGHT_BRACKET;
    case Key::UGrave:
        return KEY_APOSTROPHE;
    case Key::Asterisk:
        return KEY_BACKSLASH;
    case Key::Comma:
        return KEY_M;
    case Key::Semicolon:
        return KEY_COMMA;
    case Key::Colon:
        return KEY_PERIOD;
    case Key::Exclamation:
        return KEY_SLASH;
    case Key::F1:
        return KEY_F1;
    case Key::F2:
        return KEY_F2;
    case Key::F3:
        return KEY_F3;
    case Key::F4:
        return KEY_F4;
    case Key::F5:
        return KEY_F5;
    case Key::F6:
        return KEY_F6;
    case Key::F7:
        return KEY_F7;
    case Key::F8:
        return KEY_F8;
    case Key::F9:
        return KEY_F9;
    case Key::F10:
        return KEY_F10;
    case Key::F11:
        return KEY_F11;
    case Key::F12:
        return KEY_F12;
    case Key::Escape:
        return KEY_ESCAPE;
    case Key::Enter:
        return KEY_ENTER;
    case Key::Tab:
        return KEY_TAB;
    case Key::Backspace:
        return KEY_BACKSPACE;
    case Key::Insert:
        return KEY_INSERT;
    case Key::Delete:
        return KEY_DELETE;
    case Key::Home:
        return KEY_HOME;
    case Key::End:
        return KEY_END;
    case Key::PageUp:
        return KEY_PAGE_UP;
    case Key::PageDown:
        return KEY_PAGE_DOWN;
    case Key::CapsLock:
        return KEY_CAPS_LOCK;
    case Key::ScrollLock:
        return KEY_SCROLL_LOCK;
    case Key::NumLock:
        return KEY_NUM_LOCK;
    case Key::PrintScreen:
        return KEY_PRINT_SCREEN;
    case Key::Pause:
        return KEY_PAUSE;
    case Key::LeftControl:
        return KEY_LEFT_CONTROL;
    case Key::LeftAlt:
        return KEY_LEFT_ALT;
    case Key::LeftSuper:
        return KEY_LEFT_SUPER;
    case Key::RightShift:
        return KEY_RIGHT_SHIFT;
    case Key::RightControl:
        return KEY_RIGHT_CONTROL;
    case Key::RightAlt:
        return KEY_RIGHT_ALT;
    case Key::RightSuper:
        return KEY_RIGHT_SUPER;
    case Key::Menu:
        return KEY_KB_MENU;
    case Key::Keypad0:
        return KEY_KP_0;
    case Key::Keypad1:
        return KEY_KP_1;
    case Key::Keypad2:
        return KEY_KP_2;
    case Key::Keypad3:
        return KEY_KP_3;
    case Key::Keypad4:
        return KEY_KP_4;
    case Key::Keypad5:
        return KEY_KP_5;
    case Key::Keypad6:
        return KEY_KP_6;
    case Key::Keypad7:
        return KEY_KP_7;
    case Key::Keypad8:
        return KEY_KP_8;
    case Key::Keypad9:
        return KEY_KP_9;
    case Key::KeypadDecimal:
        return KEY_KP_DECIMAL;
    case Key::KeypadDivide:
        return KEY_KP_DIVIDE;
    case Key::KeypadMultiply:
        return KEY_KP_MULTIPLY;
    case Key::KeypadSubtract:
        return KEY_KP_SUBTRACT;
    case Key::KeypadAdd:
        return KEY_KP_ADD;
    case Key::KeypadEnter:
        return KEY_KP_ENTER;
    case Key::KeypadEqual:
        return KEY_KP_EQUAL;
    case Key::Unknown:
        break;
    }
    return KEY_NULL;
}

MouseButton RaylibRenderer::toRaylib(Mouse button)
{
    switch (button) {
    case Mouse::Left:
        return MOUSE_BUTTON_LEFT;
    case Mouse::Right:
        return MOUSE_BUTTON_RIGHT;
    case Mouse::Middle:
        return MOUSE_BUTTON_MIDDLE;
    }
    return MOUSE_BUTTON_LEFT;
}

Texture2D &RaylibRenderer::getTexture(std::string const &path)
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

Font RaylibRenderer::getFont(std::string const &fontPath)
{
    if (fontPath.empty())
        return GetFontDefault();
    auto [iterator, inserted] = _fonts.try_emplace(fontPath);
    if (inserted) {
        // Rasterized big once, then scaled down to each text's size
        Font font = LoadFontEx(fontPath.c_str(), FONT_LOAD_SIZE, nullptr, 0);
        if (font.texture.id == 0 || font.glyphCount == 0) {
            font = GetFontDefault();
        } else {
            SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
        }
        iterator->second = font;
    }
    return iterator->second;
}

void RaylibRenderer::unloadFonts()
{
    const Font builtIn = GetFontDefault();
    for (auto &[path, font] : _fonts) {
        (void)path;
        if (font.texture.id != builtIn.texture.id)
            UnloadFont(font);
    }
    _fonts.clear();
}

void RaylibRenderer::unloadTextures()
{
    for (auto &[path, texture] : _textures) {
        (void)path;
        if (texture.id != 0)
            UnloadTexture(texture);
    }
    _textures.clear();
    unloadAnimations();
    for (auto &[path, model] : _models) {
        (void)path;
        UnloadModel(model);
    }
    _models.clear();
}

RaylibRenderer::AnimationSet &RaylibRenderer::getAnimations(std::string const &path)
{
    auto [iterator, inserted] = _animations.try_emplace(path);
    if (inserted) {
        iterator->second.animations = LoadModelAnimations(path.c_str(), &iterator->second.count);
        if (iterator->second.animations == nullptr || iterator->second.count == 0)
            throw EngineError("No animations found in " + path);
    }
    return iterator->second;
}

void RaylibRenderer::unloadAnimations()
{
    for (auto &[path, animationSet] : _animations) {
        (void)path;
        if (animationSet.animations != nullptr)
            UnloadModelAnimations(animationSet.animations, animationSet.count);
    }
    _animations.clear();
}
