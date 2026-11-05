/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** LocalPeer
*/

#pragma once
#include "NetEvents.hpp"

// Client only: the id the server gave this client, NO_PEER while not connected
struct LocalPeer {
    PeerId id = NO_PEER;
};
