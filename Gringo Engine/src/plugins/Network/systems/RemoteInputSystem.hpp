/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RemoteInputSystem
*/

#pragma once
#include "Events.hpp"
#include "NetEvents.hpp"
#include "NetLog.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include <cstdint>
#include <unordered_map>

// Server, PrepUpdate after PlayerSystem: keeps each peer's newest InputState (an older tick is
// ignored), forgets it when the peer disconnects, then writes it as RemoteInput on every entity
// the peer owns (RemoteInput{} for a peer with no input)
class RemoteInputSystem : public System
{
  public:
    void update(Scene &scene) override;

  private:
    struct Latest {
        std::uint32_t tick;
        std::uint32_t bits;
    };

    void receive(Scene &scene);
    void apply(Scene &scene) const;

    std::unordered_map<PeerId, Latest> _inputs;
    EventReader<PeerDisconnected> _disconnects;
    LogOnce _log;
};
