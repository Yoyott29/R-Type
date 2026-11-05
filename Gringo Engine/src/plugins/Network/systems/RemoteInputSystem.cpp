/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** RemoteInputSystem
*/

#include "RemoteInputSystem.hpp"
#include "ActionInput.hpp"
#include "DecodeOrLog.hpp"
#include "Messages.hpp"
#include "NetComponents.hpp"
#include "Networked.hpp"
#include "Packet.hpp"
#include "ReservedInbox.hpp"
#include <bitset>
#include <limits>
#include <optional>

static_assert(MAX_ACTIONS <= std::numeric_limits<decltype(InputStateMessage::bits)>::digits,
              "every action must fit in the InputState bits");

void RemoteInputSystem::update(Scene &scene)
{
    // Messages first: a peer that sent input and left in the same frame ends with none
    receive(scene);
    _disconnects.read(scene, [&](PeerDisconnected const &event) { _inputs.erase(event.peer); });
    apply(scene);
}

void RemoteInputSystem::receive(Scene &scene)
{
    for (NetMessage const &message : scene.getResource<ReservedInbox>().messages) {
        if (message.type != static_cast<std::uint16_t>(ReservedType::InputState))
            continue;
        std::optional<InputStateMessage> const decoded =
            decodeOrLog<InputStateMessage>(_log, message, "InputState");
        if (!decoded)
            continue;
        auto const found = _inputs.find(message.peer);
        if (found != _inputs.end() && decoded->tick < found->second.tick)
            continue;
        _inputs[message.peer] = Latest{decoded->tick, decoded->bits};
    }
}

void RemoteInputSystem::apply(Scene &scene) const
{
    for (Entity entity : scene.view<Networked>()) {
        PeerId const owner = scene.getComponent<Networked>(entity).owner;
        if (owner == NO_PEER)
            continue;
        // RemoteInput{} for a peer with no input
        RemoteInput input{};
        if (auto const found = _inputs.find(owner); found != _inputs.end())
            input.input.active = std::bitset<MAX_ACTIONS>(found->second.bits);
        if (scene.hasComponent<RemoteInput>(entity))
            scene.getComponent<RemoteInput>(entity) = input;
        else
            scene.addComponent(entity, input);
    }
}
