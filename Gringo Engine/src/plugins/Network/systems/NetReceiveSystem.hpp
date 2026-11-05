/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetReceiveSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

// Drains the socket's receive queue into NetTransport, then publishes its events
class NetReceiveSystem : public System
{
  public:
    void update(Scene &scene) override;
};
