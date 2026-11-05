/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** ReservedInbox
*/

#pragma once
#include "NetEvents.hpp"
#include "NetworkConfig.hpp"
#include "Packet.hpp"
#include <vector>

// The reserved messages received this frame that this side accepts (see acceptsReserved),
// refilled by NetReceiveSystem
struct ReservedInbox {
    std::vector<NetMessage> messages;
};

// Which side a reserved message is for: inputs go up to the server, the world comes down to clients
inline bool acceptsReserved(NetRole role, ReservedType type)
{
    switch (type) {
    case ReservedType::InputState:
        return role == NetRole::Server;
    case ReservedType::EntityState:
    case ReservedType::Despawn:
    case ReservedType::PeerLeft:
        return role == NetRole::Client;
    }
    return false;
}
