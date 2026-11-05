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

class AudioSystem : public System
{
  public:
    void Start(Scene &scene) override;
    void update(Scene &scene) override;

  private:
    ma_engine _engine;
    bool _engineReady = false;
    std::unordered_map<Entity, ma_sound> _sounds;
};
