/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** PluginApi
*/

#pragma once
#include "Application.hpp"

// Every plugin library exports one C function with this name, so the loader finds it without C++
// name mangling. It returns a new plugin, deleted by the Application through IPlugin's destructor
#define PLUGIN_ENTRY_POINT "createPlugin"

#if defined(_WIN32)
#define PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
#define PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
#endif

using CreatePluginFunction = IPlugin *(*)();
