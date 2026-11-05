/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** DecodeOrLog
*/

#pragma once
#include "NetEvents.hpp"
#include "NetLog.hpp"
#include <optional>
#include <string>

// The decoded message, or nullopt logged once per type
template <typename Message>
std::optional<Message> decodeOrLog(LogOnce &log, NetMessage const &message, char const *name)
{
    std::optional<Message> decoded = Message::decode(message.payload);
    if (!decoded)
        log("decode" + std::to_string(message.type), std::string("dropped a malformed ") + name);
    return decoded;
}
