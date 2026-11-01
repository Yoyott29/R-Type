/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ServerNetSystem
*/

#pragma once
#include "Events.hpp"
#include "NetEvents.hpp"
#include "Scene.hpp"
#include "System.hpp"
// Handles connections, disconnections and inputs from the network events
class ServerNetSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    EventReader<PeerConnected> _connected;
    EventReader<PeerDisconnected> _disconnected;
    EventReader<NetMessage> _messages;
};
