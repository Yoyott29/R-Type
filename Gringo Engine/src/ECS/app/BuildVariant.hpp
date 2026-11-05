/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** BuildVariant
*/

#pragma once

#include <string>

// An executable the build makes from this project, e.g. a headless one
struct BuildVariant {
    std::string name;
    bool headless = false;
};
