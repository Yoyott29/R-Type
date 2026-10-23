/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ClientNetSystem
*/

#pragma once
#include "Events.hpp"
#include "NetEvents.hpp"
#include "Scene.hpp"
#include "System.hpp"

// Handles the Welcome, the Snapshots and the disconnection from the network events
class ClientNetSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    EventReader<PeerDisconnected> _disconnected;
    EventReader<NetMessage> _messages;
};
