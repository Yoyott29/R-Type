/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** AudioSourceSystem
*/

#pragma once
#include "AudioSource.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "miniaudio.h"
#include <unordered_map>
#include <unordered_set>

class AudioSystem : public System
{
  public:
    ~AudioSystem() override;
    void Start(Scene &scene) override;
    void update(Scene &scene) override;

  private:
    void updateSound(Entity entity, const AudioSource &source);
    void createSound(Entity entity, const AudioSource &source);
    static void applySettings(ma_sound &sound, const AudioSource &source);
    void releaseRemovedSounds();

    ma_engine _engine;
    bool _engineReady = false;
    std::unordered_map<Entity, ma_sound> _sounds;
    std::unordered_set<Entity> _failed;
};
