/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** TryConnect
*/

#pragma once
#include "Scene.hpp"
#include "Socket.hpp"
#include <chrono>

class TryConnect : public System
{
  public:
    void update(Scene &scene) override;

  private:
    static constexpr std::chrono::seconds CONNECT_RETRY_DELAY{1};

    static bool openSocket(Socket &socket, asio::error_code &ec);
};
