/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** CommandLine
*/

#pragma once
#include <string>
#include <vector>

// Scene resource: the arguments the game was started with, without the program name. Plugins
// read the ones they understand, e.g. Network takes "[host] port"
struct CommandLine {
    std::vector<std::string> arguments;
};
