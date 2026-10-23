/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RTypeClientPlugin
*/

#pragma once
#include "Application.hpp"
#include <cstdint>
#include <string>

// Client game layer: sends the inputs and draws the snapshots. Add it after the renderer (Sprite,
// KeyboardInput) and the NetworkPlugin
class RTypeClientPlugin : public IPlugin
{
  public:
    RTypeClientPlugin(std::string serverAddress, std::uint16_t serverPort);

    void build(Application &app) override;
    void cleanup(Application &app) override;

  private:
    std::string _server;
};
