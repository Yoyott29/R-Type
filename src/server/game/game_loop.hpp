/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** game_loop
*/

#pragma once
#include "input/input_queue.hpp"
#include <atomic>
#include <chrono>
#include <cstdint>

namespace rtype_server
{
class GameLoop
{
  public:
    GameLoop(InputQueue &inputs, std::atomic<bool> &running);
    void run();

  private:
    void executeInput(uint8_t input);
    void debugPrintInput(uint8_t input); // TEST: a supprimer

    static constexpr std::chrono::milliseconds TICK_DURATION{1000};

    InputQueue &_inputs;
    std::atomic<bool> &_running;
};
} // namespace rtype_server
