/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ReadSocket
*/

#pragma once
#include "Scene.hpp"
#include "Socket.hpp"
#include "rtype_protocol.hpp"
#include <cstddef>
#include <cstdint>

class ReadSocket : public System
{
  public:
    void update(Scene &scene) override;

  private:
    static void handlePacket(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet,
                             std::size_t len);
    static char const *rejectReason(uint8_t reason);
};
