/*
** EPITECH PROJECT, 2026
** R-Type-Mirror
** File description:
** ComponentDescriptor
*/

#pragma once
#include "ColorRGBA.hpp"
#include <string>
#include <vector>

#define NO_SCRIPT "NONE"

enum class valueType {
    Int,
    Float,
    String,
    Bool,
    Color,
    Shape,
    Mask,
    Script,
};

struct PropertyDescriptor {
    std::string name;
    valueType type;
    std::string defaultValue;
};

struct ComponentDescriptor {
    std::string name;
    std::vector<PropertyDescriptor> properties;
    ColorRGBA color{58, 57, 61, 255};
    std::string plugin;
};

template <typename Component> struct ComponentDescription {
    static constexpr bool isDescribed = false;
};
