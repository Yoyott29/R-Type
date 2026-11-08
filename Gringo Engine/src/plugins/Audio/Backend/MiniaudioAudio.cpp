/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** MiniaudioAudio
*/

#include "MiniaudioAudio.hpp"

MiniaudioAudio::MiniaudioAudio() : _engine{}
{
    _ready = ma_engine_init(nullptr, &_engine) == MA_SUCCESS;
}

MiniaudioAudio::~MiniaudioAudio()
{
    for (auto &[id, sound] : _sounds)
        ma_sound_uninit(&sound);
    if (_ready)
        ma_engine_uninit(&_engine);
}

std::optional<std::size_t> MiniaudioAudio::load(std::string const &path)
{
    if (!_ready)
        return std::nullopt;
    const std::size_t id = _nextId++;
    ma_sound &sound = _sounds[id];
    if (ma_sound_init_from_file(&_engine, path.c_str(), 0, nullptr, nullptr, &sound) !=
        MA_SUCCESS) {
        _sounds.erase(id);
        return std::nullopt;
    }
    return id;
}

void MiniaudioAudio::unload(std::size_t sound)
{
    const auto found = _sounds.find(sound);
    if (found == _sounds.end())
        return;
    ma_sound_uninit(&found->second);
    _sounds.erase(found);
}

void MiniaudioAudio::play(std::size_t sound)
{
    const auto found = _sounds.find(sound);
    if (found == _sounds.end())
        return;
    ma_sound_seek_to_pcm_frame(&found->second, 0);
    ma_sound_start(&found->second);
}

void MiniaudioAudio::stop(std::size_t sound)
{
    const auto found = _sounds.find(sound);
    if (found != _sounds.end())
        ma_sound_stop(&found->second);
}

bool MiniaudioAudio::isPlaying(std::size_t sound) const
{
    const auto found = _sounds.find(sound);
    return found != _sounds.end() && ma_sound_is_playing(&found->second);
}

bool MiniaudioAudio::isAtEnd(std::size_t sound) const
{
    const auto found = _sounds.find(sound);
    return found != _sounds.end() && ma_sound_at_end(&found->second);
}

void MiniaudioAudio::setVolume(std::size_t sound, float volume)
{
    const auto found = _sounds.find(sound);
    if (found != _sounds.end())
        ma_sound_set_volume(&found->second, volume);
}

void MiniaudioAudio::setLooping(std::size_t sound, bool looping)
{
    const auto found = _sounds.find(sound);
    if (found != _sounds.end())
        ma_sound_set_looping(&found->second, static_cast<ma_bool32>(looping));
}
