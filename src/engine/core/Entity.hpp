/*
** EPITECH PROJECT, 2026
** G-ING-500-LIL-5-1-rtype-4
** File description:
** Entity
*/

#pragma once
#include <cstdint>
#include <bitset>

#define MAX_ENTITIES 4096
#define MAX_COMPONENTS 32
#define NULL_INDEX static_cast<std::size_t>(-1)

using Entity = std::uint32_t;
using ComponentType = std::uint8_t;
using ComponentSet = std::bitset<MAX_COMPONENTS>;
