/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** input_queue
*/

#pragma once
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <optional>
#include <queue>

namespace rtype_server
{
class InputQueue
{
  public:
    void push(uint8_t input);
    std::optional<uint8_t> pop();

  private:
    static constexpr std::size_t MAX_STORED_INPUTS = 2;

    std::mutex _mutex;
    std::queue<uint8_t> _inputs;
};
} // namespace rtype_server
