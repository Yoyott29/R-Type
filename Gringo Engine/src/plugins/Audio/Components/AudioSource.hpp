/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** AudioSource
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include <string>

// Plays the sound at `path`. With PlayOnStart it starts as soon as it is loaded.
// From a script: scene.getComponent<AudioSource>(entity).play() (from the start) / .stop()
struct AudioSource {
    std::string path;
    int volume = 100;
    bool loop = false;

    bool playing = false;
    bool restart = false;

    void play()
    {
        playing = true;
        restart = true;
    }
    void stop()
    {
        playing = false;
        restart = false;
    }
    bool isPlaying() const { return playing; }
};

template <> struct ComponentDescription<AudioSource> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"AudioSource",
                {
                    {"Path", valueType::File, NO_FILE, {"assets", ".mp3"}},
                    {"Volume", valueType::Int, "100"},
                    {"Loop", valueType::Bool, "False"},
                    {"PlayOnStart", valueType::Bool, "True"},
                },
                {58, 53, 63, 255},
                ""};
    }
};
