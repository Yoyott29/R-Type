/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Physics
*/

#pragma once
#include "Application.hpp"

class Physics : public IPlugin
{
  public:
    Physics() = default;
    ~Physics() = default;
    void build(Application &app) override;
    void cleanup(Application &app) override;
};
