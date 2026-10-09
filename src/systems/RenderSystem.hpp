/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RenderSystem
*/

#pragma once
#include "System.hpp"
#include "Scene.hpp"
#include "IRenderer.hpp"

class RenderSystem : public System {
    public:
        void update(Scene &scene, IRenderer &renderer);
};
