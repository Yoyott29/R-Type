/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Client
*/

#pragma once
#include "Application.hpp"
#include "Socket.hpp"

class Client : public IPlugin
{
  public:
    void build(Application &app) override;
    void cleanup(Application &app) override;
};
