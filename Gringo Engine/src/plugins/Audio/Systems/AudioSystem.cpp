/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** AudioSourceSystem
*/

#define MINIAUDIO_IMPLEMENTATION
#include "AudioSystem.hpp"

AudioSystem::~AudioSystem()
{
    for (auto &[entity, sound] : _sounds)
        ma_sound_uninit(&sound);
    if (_engineReady)
        ma_engine_uninit(&_engine);
}

void AudioSystem::Start(Scene &scene)
{
    (void)scene;
    _engineReady = ma_engine_init(nullptr, &_engine) == MA_SUCCESS;
}

void AudioSystem::update(Scene &scene)
{
    if (!_engineReady)
        return;
    releaseRemovedSounds();
    for (Entity entity : _entities)
        updateSound(entity, scene.getComponent<AudioSource>(entity));
}

void AudioSystem::updateSound(Entity entity, const AudioSource &source)
{
    if (_failed.contains(entity))
        return;
    if (!_sounds.contains(entity)) {
        createSound(entity, source);
        return;
    }
    applySettings(_sounds.at(entity), source);
}

void AudioSystem::createSound(Entity entity, const AudioSource &source)
{
    ma_sound &sound = _sounds[entity];
    if (ma_sound_init_from_file(&_engine, source.path.c_str(), 0, nullptr, nullptr, &sound) !=
        MA_SUCCESS) {
        _sounds.erase(entity);
        _failed.insert(entity);
        return;
    }
    applySettings(sound, source);
    ma_sound_start(&sound);
}

void AudioSystem::applySettings(ma_sound &sound, const AudioSource &source)
{
    ma_sound_set_volume(&sound, static_cast<float>(source.volume) / 100.0f);
    ma_sound_set_looping(&sound, static_cast<ma_bool32>(source.loop));
}

void AudioSystem::releaseRemovedSounds()
{
    for (auto it = _sounds.begin(); it != _sounds.end();) {
        if (_entities.contains(it->first)) {
            ++it;
        } else {
            ma_sound_uninit(&it->second);
            it = _sounds.erase(it);
        }
    }
    for (auto it = _failed.begin(); it != _failed.end();) {
        if (_entities.contains(*it))
            ++it;
        else
            it = _failed.erase(it);
    }
}
