/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** main
*/

#include "Application.hpp"
#include <iostream>

int main()
{
    try {
        Application application;
        application.run();
    } catch (std::exception const &error) {
        std::cerr << "Fatal error: " << error.what() << "\n";
        return 84;
    }
    return 0;
}
