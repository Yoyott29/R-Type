/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ButtonClickSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Runs in PrepUpdate: calls the script of every UIButton under a left click
class ButtonClickSystem : public System
{
  public:
    void update(Scene &scene) override;
};
