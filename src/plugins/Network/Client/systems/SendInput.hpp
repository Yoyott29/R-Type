/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** SendInput
*/

#pragma once
#include "KeyboardInput.hpp"
#include "Scene.hpp"
#include "Socket.hpp"
#include "rtype_protocol.hpp"
#include <array>
#include <iostream>

class SendInput : public System
{
  public:
    void update(Scene &scene) override
    {
        Socket &socket = scene.getResource<Socket>();
        if (!socket.connected)
            return;

        KeyboardInput const &keyboard = scene.getResource<KeyboardInput>();
        uint8_t inputs = 0;
        if (keyboard.isDown(Key::Z) || keyboard.isDown(Key::Up))
            inputs |= rtype_protocol::UP;
        if (keyboard.isDown(Key::S) || keyboard.isDown(Key::Down))
            inputs |= rtype_protocol::DOWN;
        if (keyboard.isDown(Key::Q) || keyboard.isDown(Key::Left))
            inputs |= rtype_protocol::LEFT;
        if (keyboard.isDown(Key::D) || keyboard.isDown(Key::Right))
            inputs |= rtype_protocol::RIGHT;
        if (keyboard.isDown(Key::Space))
            inputs |= rtype_protocol::CHARGE;

        // [INPUT, tick (u32), InputBits, lastSnapshotTick (u32)]
        std::array<uint8_t, 10> packet{};
        packet[0] = rtype_protocol::INPUT;
        rtype_protocol::writeValue<uint32_t>(&packet[1], socket.tick);
        packet[5] = inputs;
        rtype_protocol::writeValue<uint32_t>(&packet[6], socket.lastSnapshotTick);

        asio::error_code ec;
        socket.socket.send(asio::buffer(packet), 0, ec);
        if (ec && ec != asio::error::would_block)
            std::cerr << "SendInput: " << ec.message() << "\n";
        socket.tick++;
    }
};
