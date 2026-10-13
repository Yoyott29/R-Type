/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** GetInput
*/

#pragma once
#include "KeyboardInput.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "AppAlive.hpp"
#include <array>
#include <raylib.h>
#include <utility>

class GetInput: public System{
    public:
    void update(Scene &scene) {
        KeyboardInput &keyboard = scene.getResource<KeyboardInput>();

        keyboard.pressed.reset();
        for (auto const &[raylibKey, key] : KEY_BINDINGS)
            if (IsKeyDown(raylibKey))
                keyboard.pressed.set(static_cast<std::size_t>(key));
        if (WindowShouldClose())
            scene.getResource<AppAlive>().alive = false;
    }

    private:
        static constexpr std::array<std::pair<KeyboardKey, Key>, 9> KEY_BINDINGS = {{
            {KEY_Z, Key::Z},
            {KEY_S, Key::S},
            {KEY_Q, Key::Q},
            {KEY_D, Key::D},
            {KEY_UP, Key::Up},
            {KEY_DOWN, Key::Down},
            {KEY_LEFT, Key::Left},
            {KEY_RIGHT, Key::Right},
            {KEY_SPACE, Key::Space},
        }};
};
