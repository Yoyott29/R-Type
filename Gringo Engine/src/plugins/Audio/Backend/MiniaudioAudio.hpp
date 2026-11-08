/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** MiniaudioAudio
*/

#pragma once
#include "IAudio.hpp"
#include "miniaudio.h"
#include <unordered_map>

// IAudio implemented with miniaudio: the only file that knows about the library
class MiniaudioAudio : public IAudio
{
  public:
    MiniaudioAudio();
    ~MiniaudioAudio() override;
    // The sounds point to the engine: it can not be copied
    MiniaudioAudio(MiniaudioAudio const &) = delete;
    MiniaudioAudio &operator=(MiniaudioAudio const &) = delete;

    std::optional<std::size_t> load(std::string const &path) override;
    void unload(std::size_t sound) override;

    void play(std::size_t sound) override;
    void stop(std::size_t sound) override;
    bool isPlaying(std::size_t sound) const override;
    bool isAtEnd(std::size_t sound) const override;

    void setVolume(std::size_t sound, float volume) override;
    void setLooping(std::size_t sound, bool looping) override;

  private:
    ma_engine _engine;
    bool _ready = false;
    // Map nodes do not move: miniaudio keeps pointers to its sounds
    std::unordered_map<std::size_t, ma_sound> _sounds;
    std::size_t _nextId = 0;
};
