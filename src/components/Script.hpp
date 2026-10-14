/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** Script
*/

#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include "Entity.hpp"
#include "IRenderer.hpp"
#include "Scene.hpp"
#include <functional>

struct Script {
    std::function<void(Scene &, Entity &)> update;
};

#endif /* !SCRIPT_HPP_ */
