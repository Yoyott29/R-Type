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

enum class valueType {
    Int,
    Float,
    String,
    Bool,
    Color,
    Shape,
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
};

template <typename Component> struct ComponentDescription {
    static constexpr bool isDescribed = false;
};
