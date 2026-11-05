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

// Value of a File property when no file is chosen
#define NO_FILE "NONE"

// How the editor edits a property. The numbers are saved in .gringo files
enum class valueType {
    Int = 0,
    Float = 1,
    String = 2,
    Bool = 3,
    Color = 4,
    // One of PropertyDescriptor::options, e.g. {"Box", "Circle"}
    Choice = 5,
    // A file of the project: options = {folder, extensions...}, e.g. {"assets", ".png", ".jpg"}
    File = 7,
};

// The editor shows a property only while another property of its component has a value, e.g.
// {"Shape", "Box"}. An empty property name: always shown
struct PropertyCondition {
    std::string property;
    std::string value;
};

// One editable value: a component property, or a plugin setting (IPlugin::settings)
struct PropertyDescriptor {
    std::string name;
    valueType type;
    std::string defaultValue;
    std::vector<std::string> options = {};
    PropertyCondition visibleWhen = {};
};

struct ComponentDescriptor {
    std::string name;
    std::vector<PropertyDescriptor> properties;
    ColorRGBA color{58, 57, 61, 255};
    std::string plugin;
};

template <typename Component> struct ComponentDescription {
    static inline const bool isDescribed = false;
};
