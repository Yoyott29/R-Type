/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** AudioSourceSystem
*/

#include "AudioSystem.hpp"
#include "MiniaudioAudio.hpp"

void AudioSystem::Start(Scene &scene)
{
    (void)scene;
    _audio = std::make_unique<MiniaudioAudio>();
}

void AudioSystem::update(Scene &scene)
{
    if (!_audio)
        return;
    releaseRemovedSounds();
    for (Entity entity : _entities)
        updateSound(entity, scene.getComponent<AudioSource>(entity));
}

void AudioSystem::updateSound(Entity entity, AudioSource &source)
{
    if (_failed.contains(entity))
        return;
    if (!_sounds.contains(entity)) {
        const std::optional<std::size_t> sound = _audio->load(source.path);
        if (!sound) {
            _failed.insert(entity);
            return;
        }
        _sounds[entity] = *sound;
    }
    const std::size_t sound = _sounds.at(entity);
    _audio->setVolume(sound, static_cast<float>(source.volume) / 100.0f);
    _audio->setLooping(sound, source.loop);
    syncPlayback(sound, source);
}

void AudioSystem::syncPlayback(std::size_t sound, AudioSource &source)
{
    // play(): from the start, even if it is already playing
    if (source.restart) {
        _audio->play(sound);
        source.restart = false;
        return;
    }
    if (!source.playing) {
        if (_audio->isPlaying(sound))
            _audio->stop(sound);
        return;
    }
    // A sound that does not loop is done once it reaches its end
    if (!_audio->isPlaying(sound) && _audio->isAtEnd(sound))
        source.playing = false;
}

void AudioSystem::releaseRemovedSounds()
{
    for (auto it = _sounds.begin(); it != _sounds.end();) {
        if (_entities.contains(it->first)) {
            ++it;
        } else {
            _audio->unload(it->second);
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
