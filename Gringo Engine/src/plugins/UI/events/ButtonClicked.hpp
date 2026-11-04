/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ButtonClicked
*/

#pragma once
#include "Entity.hpp"

// Emitted on the EventBus when a UIButton is clicked
struct ButtonClicked {
    Entity entity;
};
