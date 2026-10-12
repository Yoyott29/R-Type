/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** network
*/

#pragma once
#include "input/input_queue.hpp"
#include <atomic>
#include <cstdint>

namespace rtype_server
{
class Network
{
  public:
    Network(uint16_t port, InputQueue &inputs, std::atomic<bool> &running);
    ~Network();
    void receiveLoop();

  private:
    int _socket = -1;
    InputQueue &_inputs;
    std::atomic<bool> &_running;
};
} // namespace rtype_server
