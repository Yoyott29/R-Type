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
#include <utility>
#include <vector>

class ReadSocket : public System
{
  public:
    void update(Scene &scene) override;

  private:
    void handlePacket(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet,
                      std::size_t len);
    void applySnapshot(Scene &scene, Socket &socket);
    static void createEntities(Scene &scene, rtype_protocol::Packet const &packet, std::size_t len);
    static char const *rejectReason(uint8_t reason);

    void handleConnectAccept(Scene &scene, Socket &socket,rtype_protocol::Packet const &packet, std::size_t len);
    void handleConnectReject(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet, std::size_t len);
    void handlePlayerLeft(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet, std::size_t len);
    void handleGameStarted(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet, std::size_t len);
    void handleSnapshot(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet, std::size_t len);
    void handleDelta(Scene &scene, Socket &socket, rtype_protocol::Packet const &packet, std::size_t len);

    std::vector<std::pair<rtype_protocol::Packet, std::size_t>> _snapshotParts;
    uint32_t _snapshotTick = 0;
    uint32_t _displayedTick = 0;
};
