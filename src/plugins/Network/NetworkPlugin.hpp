/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** NetworkPlugin
*/

#pragma once
#include "Application.hpp"
#include "NetworkConfig.hpp"
#include "Scene.hpp"

// Transport-only UDP networking: adds the socket, NetTransport and the event queues to a scene.
// Reads its NetworkConfig from the scene resource the executable adds before loading it
class NetworkPlugin : public IPlugin
{
  public:
    void build(Application &app) override;
    void cleanup(Application &app) override;

  private:
    void build(Scene &scene);
    void cleanup(Scene &scene);
};
