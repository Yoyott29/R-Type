/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** AudioSource
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include <string>

struct AudioSource {
    std::string path;
    int volume = 100;
    bool loop = false;
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
                },
                {58, 53, 63, 255},
                ""};
    }
};
