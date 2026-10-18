/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Client
*/

#pragma once
#include "Application.hpp"
#include "Socket.hpp"
#include <cstdint>
#include <string>

class Client : public IPlugin
{
  public:
    Client(const std::string &serverAddress, uint16_t serverPort);
    void build(Application &app) override;
    void cleanup(Application &app) override;

  private:
    std::string _serverAddress;
    uint16_t _serverPort;
};
