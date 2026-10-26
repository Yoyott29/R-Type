/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** RTypeServerPlugin
*/

#pragma once
#include "Application.hpp"

class RTypeServerPlugin : public IPlugin
{
  public:
    void build(Application &app) override;
    void cleanup(Application &app) override;
};
