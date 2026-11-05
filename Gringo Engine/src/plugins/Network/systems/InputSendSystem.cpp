/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** InputSendSystem
*/

#include "InputSendSystem.hpp"
#include "ActionInput.hpp"
#include "GameClock.hpp"
#include "Messages.hpp"
#include "NetEvents.hpp"
#include "NetTransport.hpp"
#include "Packet.hpp"
#include <limits>

static_assert(MAX_ACTIONS <= std::numeric_limits<decltype(InputStateMessage::bits)>::digits,
              "every action must fit in the InputState bits");

void InputSendSystem::update(Scene &scene)
{
    auto &transport = scene.getResource<NetTransport>();
    if (transport.state() != ConnectionState::Connected)
        return;
    std::uint64_t const tick = scene.getResource<GameClock>().tick;
    if (_lastSent == tick)
        return;
    _lastSent = tick;
    ActionInput actions;
    if (scene.hasResource<ActionInput>())
        actions = scene.getResource<ActionInput>();
    auto const bits = static_cast<std::uint32_t>(actions.active.to_ulong());
    transport.sendReserved(SERVER_PEER, ReservedType::InputState,
                           InputStateMessage{static_cast<std::uint32_t>(tick), bits}.encode());
}
