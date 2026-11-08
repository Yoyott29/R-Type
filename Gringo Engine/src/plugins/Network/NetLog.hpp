/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** NetLog
*/

#pragma once
#include <iostream>
#include <string>
#include <unordered_set>

// Prints a message to stderr the first time its key is seen
class LogOnce
{
  public:
    void operator()(std::string const &key, std::string const &message)
    {
        if (_seen.insert(key).second)
            std::cerr << "NetworkPlugin: " << message << "\n";
    }

  private:
    std::unordered_set<std::string> _seen;
};
