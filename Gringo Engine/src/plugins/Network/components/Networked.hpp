/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** Networked
*/

#pragma once
#include "ComponentDescriptor.hpp"
#include "EngineError.hpp"
#include "NetEvents.hpp"
#include <cstdint>
#include <string>

// The side that keeps the entity: Server only, Client only, or Replicated (server keeps it,
// clients get a mirror)
enum class NetMode : std::uint8_t { Server, Client, Replicated };

struct Networked {
    NetMode mode = NetMode::Replicated;
    // What the client instantiates as the mirror
    std::string prefab;
    // The peer controlling the entity
    PeerId owner = NO_PEER;

    // Throws EngineError on an unknown text
    static NetMode parseMode(std::string const &text)
    {
        if (text == "Server")
            return NetMode::Server;
        if (text == "Client")
            return NetMode::Client;
        if (text == "Replicated")
            return NetMode::Replicated;
        throw EngineError("Networked: unknown Mode " + text);
    }
};

// Header colour of the Networked component in the editor
inline const ColorRGBA NETWORKED_EDITOR_COLOR{53, 66, 70, 255};

template <> struct ComponentDescription<Networked> {
    static inline const bool isDescribed = true;

    static ComponentDescriptor describe()
    {
        return {"Networked",
                {
                    {"Mode", valueType::Choice, "Replicated", {"Server", "Client", "Replicated"}},
                    {"Prefab", valueType::String, ""},
                },
                NETWORKED_EDITOR_COLOR,
                ""};
    }
};
