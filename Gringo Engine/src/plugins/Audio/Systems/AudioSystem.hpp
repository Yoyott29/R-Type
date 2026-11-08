/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** AudioSourceSystem
*/

#pragma once
#include "AudioSource.hpp"
#include "IAudio.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>

// Loads the sound of each AudioSource and plays / stops it as the component asks, through IAudio
class AudioSystem : public System
{
  public:
    void Start(Scene &scene) override;
    void update(Scene &scene) override;

  private:
    void updateSound(Entity entity, AudioSource &source);
    void syncPlayback(std::size_t sound, AudioSource &source);
    void releaseRemovedSounds();

    // Frees every loaded sound when the system goes
    std::unique_ptr<IAudio> _audio;
    std::unordered_map<Entity, std::size_t> _sounds;
    std::unordered_set<Entity> _failed;
};
