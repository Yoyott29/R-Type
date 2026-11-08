/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** IAudio
*/

#pragma once
#include <cstddef>
#include <optional>
#include <string>

// The most basic functions an audio library gives: loading a sound file and playing it. An
// implementation (MiniaudioAudio) wraps one library; the Audio plugin's systems only call this, so
// the library can be changed without touching them
class IAudio
{
  public:
    virtual ~IAudio() = default;

    // Loads a sound file (.mp3, .wav...) and gives the number the other functions take for it.
    // Empty if it can not be loaded or there is no audio device
    virtual std::optional<std::size_t> load(std::string const &path) = 0;
    virtual void unload(std::size_t sound) = 0;

    // From the start, even if it is already playing
    virtual void play(std::size_t sound) = 0;
    virtual void stop(std::size_t sound) = 0;
    virtual bool isPlaying(std::size_t sound) const = 0;
    // True once a sound that does not loop has played to its end
    virtual bool isAtEnd(std::size_t sound) const = 0;

    // 0 (silent) to 1 (as loud as the file)
    virtual void setVolume(std::size_t sound, float volume) = 0;
    virtual void setLooping(std::size_t sound, bool looping) = 0;
};
