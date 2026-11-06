/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Animator
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include <string>

// Plays animations (.anim files of the project's assets, one animation each) on the entity's
// Sprite: the AnimationSystem sets the Sprite's texture and region to the current frame.
// path is the animation it starts with; game code switches with AnimationSystem::play(animator,
// "run"), which plays run.anim from the same folder
struct Animator {
    // The starting .anim file, resolved in the project's assets folder
    std::string path;
    // Name of the animation playing (its file without .anim); empty: the starting one
    std::string current;
    int frame = 0;
    float elapsed = 0.0f;
    // An animation with "loop": false stopped on its last frame
    bool finished = false;
    // Multiplies the animations' fps (0: paused)
    float speed = 1.0f;
};

template <> struct ComponentDescription<Animator> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Animator",
                {
                    {"Animation", valueType::File, NO_FILE, {"assets", ".anim"}},
                    {"Speed", valueType::Float, "1"},
                },
                {56, 51, 70, 255},
                ""};
    }
};
