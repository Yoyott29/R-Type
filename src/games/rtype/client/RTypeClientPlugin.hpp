/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RTypeClientPlugin
*/

#pragma once
#include "Application.hpp"

// Client game layer: sends the inputs and draws the snapshots. Add it after the renderer (Sprite,
// KeyboardInput) and the NetworkPlugin
class RTypeClientPlugin : public IPlugin
{
  public:
    void build(Application &app) override;
    void cleanup(Application &app) override;
};
