/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** NetSendSystem
*/

#pragma once
#include "Scene.hpp"
#include "System.hpp"

class NetTransport;
class UdpSocket;

// Flushes the transport's outgoing queues through the socket, at the current time
void flushToSocket(NetTransport &transport, UdpSocket &socket);

// Packs and flushes the outgoing queues through the socket
class NetSendSystem : public System
{
  public:
    void update(Scene &scene) override;
};
