/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SendInputSystem
*/

#include "SendInputSystem.hpp"
#include "ClientSession.hpp"
#include "KeyboardInput.hpp"
#include "NetTransport.hpp"
#include "RTypeMessages.hpp"
#include "rtype_protocol.hpp"

namespace
{
constexpr PeerId SERVER_PEER = 0;

std::uint8_t buttonsOf(KeyboardInput const &keyboard)
{
    std::uint8_t buttons = 0;
    if (keyboard.isDown(Key::Z) || keyboard.isDown(Key::Up))
        buttons |= rtype_protocol::UP;
    if (keyboard.isDown(Key::S) || keyboard.isDown(Key::Down))
        buttons |= rtype_protocol::DOWN;
    if (keyboard.isDown(Key::Q) || keyboard.isDown(Key::Left))
        buttons |= rtype_protocol::LEFT;
    if (keyboard.isDown(Key::D) || keyboard.isDown(Key::Right))
        buttons |= rtype_protocol::RIGHT;
    if (keyboard.isDown(Key::Space))
        buttons |= rtype_protocol::CHARGE;
    return buttons;
}
} // namespace

void SendInputSystem::update(Scene &scene)
{
    auto &transport = scene.getResource<NetTransport>();
    if (transport.state() != ConnectionState::Connected)
        return;
    auto &session = scene.getResource<ClientSession>();
    auto const input =
        encodeInput({session.inputTick++, buttonsOf(scene.getResource<KeyboardInput>())});
    transport.send(SERVER_PEER, static_cast<std::uint16_t>(RTypeMessage::Input), input);
}
