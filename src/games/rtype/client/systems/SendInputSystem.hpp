/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SendInputSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Sends the pressed keys to the server once per frame
class SendInputSystem : public System
{
  public:
    void update(Scene &scene) override;
};
