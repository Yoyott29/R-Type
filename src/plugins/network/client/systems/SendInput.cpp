/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SendInput
*/

#include "SendInput.hpp"
#include "KeyboardInput.hpp"
#include "RTypeProtocol.hpp"
#include "Socket.hpp"
#include <array>
#include <iostream>

void SendInput::update(Scene &scene)
{
    Socket &socket = scene.getResource<Socket>();
    if (!socket.connected)
        return;

    KeyboardInput const &keyboard = scene.getResource<KeyboardInput>();
    uint8_t inputs = 0;
    if (keyboard.isDown(Key::Z) || keyboard.isDown(Key::Up))
        inputs |= RTypeProtocol::UP;
    if (keyboard.isDown(Key::S) || keyboard.isDown(Key::Down))
        inputs |= RTypeProtocol::DOWN;
    if (keyboard.isDown(Key::Q) || keyboard.isDown(Key::Left))
        inputs |= RTypeProtocol::LEFT;
    if (keyboard.isDown(Key::D) || keyboard.isDown(Key::Right))
        inputs |= RTypeProtocol::RIGHT;
    if (keyboard.isDown(Key::Space))
        inputs |= RTypeProtocol::CHARGE;

    // [INPUT, tick (u32), InputBits, lastSnapshotTick (u32)]
    std::array<uint8_t, RTypeProtocol::INPUT_SIZE> packet{};
    packet[0] = RTypeProtocol::INPUT;
    RTypeProtocol::writeValue<uint32_t>(&packet[1], socket.tick);
    packet[5] = inputs;
    RTypeProtocol::writeValue<uint32_t>(&packet[6], socket.lastSnapshotTick);

    asio::error_code ec;
    socket.socket.send(asio::buffer(packet), 0, ec);
    if (ec && ec != asio::error::would_block)
        std::cerr << "SendInput: " << ec.message() << "\n";
    socket.tick++;
}
