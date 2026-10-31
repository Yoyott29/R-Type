/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SoundSystem
*/

#pragma once
#include "IRenderer.hpp"
#include "Scene.hpp"
#include "SoundEffect.hpp"
#include "SoundManager.hpp"
#include "System.hpp"
#include <functional>
#include <string>

class SoundSystem : public System
{
  public:
    void update(Scene &scene) override
    {
        IRenderer &renderer = scene.getResource<std::reference_wrapper<IRenderer>>().get();

        updateMusic(scene, renderer);
        for (Entity entity : _entities) {
            SoundEffect &effect = scene.getComponent<SoundEffect>(entity);
            if (!effect.play)
                continue;
            effect.play = false;
            renderer.playSound(effect.path);
        }
    }

  private:
    void updateMusic(Scene &scene, IRenderer &renderer)
    {
        std::string wanted;
        if (scene.hasResource<BackgroundMusic>()) {
            BackgroundMusic const &music = scene.getResource<BackgroundMusic>();
            if (music.enabled)
                wanted = music.path;
        }
        if (wanted == _playing)
            return;

        _playing = wanted;
        if (wanted.empty())
            renderer.stopMusic();
        else
            renderer.playMusic(wanted);
    }

    std::string _playing;
};
