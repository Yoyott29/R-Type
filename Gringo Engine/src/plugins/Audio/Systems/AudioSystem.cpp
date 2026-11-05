/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** AudioSourceSystem
*/

#define MINIAUDIO_IMPLEMENTATION
#include "AudioSystem.hpp"

void AudioSystem::Start(Scene &scene)
{
    (void)scene;
    _engineReady = ma_engine_init(nullptr, &_engine) == MA_SUCCESS;
}

void AudioSystem::update(Scene &scene)
{
    if (!_engineReady)
        return;

    for (Entity entity : _entities) {
        AudioSource &source = scene.getComponent<AudioSource>(entity);
        if (_sounds.find(entity) == _sounds.end()) {
            ma_sound &sound = _sounds[entity];
            ma_result result =
                ma_sound_init_from_file(&_engine, source.path.c_str(), 0, nullptr, nullptr, &sound);
            if (result != MA_SUCCESS) {
                _sounds.erase(entity);
                continue;
            }
            ma_sound_set_volume(&sound, static_cast<float>(source.volume) / 100.0f);
            ma_sound_set_looping(&sound, source.loop ? MA_TRUE : MA_FALSE);
            ma_sound_start(&sound);
        } else {
            ma_sound &sound = _sounds[entity];
            ma_sound_set_volume(&sound, static_cast<float>(source.volume) / 100.0f);
            ma_sound_set_looping(&sound, source.loop ? MA_TRUE : MA_FALSE);
        }
    }
}
