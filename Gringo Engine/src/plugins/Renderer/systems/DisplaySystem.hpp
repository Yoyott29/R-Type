/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** DisplaySystem
*/

#pragma once
#include "IRenderer.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include <functional>

// Runs in the Display schedule: closes the frame RenderSystem opened in PostUpdate, once the
// Overlay systems (UI...) have drawn on top of it
class DisplaySystem : public System
{
  public:
    void update(Scene &scene) override
    {
        scene.getResource<std::reference_wrapper<IRenderer>>().get().endFrame();
    }
};
