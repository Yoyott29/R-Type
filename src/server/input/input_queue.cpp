/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** input_queue
*/

#include "input/input_queue.hpp"

namespace rtype_server
{
void InputQueue::push(uint8_t input)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _inputs.push(input);
    if (_inputs.size() > MAX_STORED_INPUTS)
        _inputs.pop();
}

std::optional<uint8_t> InputQueue::pop()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_inputs.empty())
        return std::nullopt;
    uint8_t input = _inputs.front();
    _inputs.pop();
    return input;
}
} // namespace rtype_server
