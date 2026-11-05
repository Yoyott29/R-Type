/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetReceiveSystem
*/

#pragma once
#include "NetLog.hpp"
#include "Scene.hpp"
#include "System.hpp"

// Drains the socket's receive queue into NetTransport, then publishes its events;
// reserved message types go to ReservedInbox, the others to Events<NetMessage>
class NetReceiveSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    LogOnce _log;
};
