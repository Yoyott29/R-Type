/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetSendSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

class NetTransport;
class UdpSocket;

// Flushes the transport's outgoing queues to the socket's I/O thread, at the current time
void flushToSocket(NetTransport &transport, UdpSocket &socket);

// Packs and flushes the outgoing queues through the socket
class NetSendSystem : public System
{
  public:
    void update(Scene &scene) override;
};
