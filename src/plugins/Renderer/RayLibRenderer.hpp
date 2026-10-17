/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RayLibRenderer
*/

#pragma once
#include "Application.hpp"
#include "GetInput.hpp"
#include "KeyboardInput.hpp"
#include "RenderSystem.hpp"
#include "Sprite.hpp"
#include <raylib.h>
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "R-Type"


class RayLibRenderer: public IPlugin {
    public:
        RayLibRenderer() = default;
        ~RayLibRenderer();
        void build(Application &app) override;
        void cleanup(Application &app) override;
};
