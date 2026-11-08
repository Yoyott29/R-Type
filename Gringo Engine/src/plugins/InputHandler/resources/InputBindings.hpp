/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** InputBindings
*/

#pragma once
#include "ActionInput.hpp"
#include "EngineError.hpp"
#include "KeyboardInput.hpp"
#include "keyboard.hpp"
#include <cstddef>
#include <map>
#include <string>

// Scene resource: which action each key triggers, several keys per action. When it exists,
// KeyboardSystem also fills the ActionInput resource from it
struct InputBindings {
    std::map<Key, std::size_t> actions;

    // Throws EngineError on an action of MAX_ACTIONS or more
    void bind(Key key, std::size_t action)
    {
        if (action >= MAX_ACTIONS)
            throw EngineError("InputBindings: action " + std::to_string(action) +
                              " is over the limit of " + std::to_string(MAX_ACTIONS));
        actions[key] = action;
    }

    // The actions of every key held in keyboard
    ActionInput resolve(KeyboardInput const &keyboard) const
    {
        ActionInput input;
        for (auto const &[key, action] : actions)
            if (keyboard.isDown(key))
                input.active.set(action);
        return input;
    }
};
