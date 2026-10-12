/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** server
*/

#pragma once
    #include <cstdint>
    #include <atomic>
    #include "input/input_queue.hpp"
    #include "network/network.hpp"
    #include "game/game_loop.hpp"

namespace rtype_server {
    class Server {
        public:
            Server();
            void run();
        private:
            static constexpr uint16_t DEFAULT_PORT = 1234;

            std::atomic<bool> _running{false};
            InputQueue _inputs;
            Network _network;
            GameLoop _gameLoop;
    };
}
