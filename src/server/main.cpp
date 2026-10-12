/*
** EPITECH PROJECT, 2026
** r-type_miroir
** File description:
** main
*/

#include "server.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    rtype_server::Server server;
    server.run();
    return 0;
}
