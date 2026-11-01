/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SoundEffect
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include <string>

struct SoundEffect {
    std::string path;
    bool play = false;
};

template <> struct ComponentDescription<SoundEffect> {
    static constexpr bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"SoundEffect",
                {
                    {"Path", valueType::String, "sound.wav"},
                    {"Play", valueType::Bool, "False"},
                },
                {70, 53, 48, 255}};
    }
};
