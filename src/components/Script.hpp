/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Script
*/

#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include "Entity.hpp"
#include "Scene.hpp"
#include <functional>
#include <memory>

enum class Timing { AT_LAUNCH, ON_UPDATE };
struct Script {
    Timing timing;
    std::function<void(Scene &, Entity &)> runScript;
    bool isRunning = false;
};

#endif /* !SCRIPT_HPP_ */
