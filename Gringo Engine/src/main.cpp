/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** main
*/

#include <iostream>
#include <raylib.h>
#include "Menu.hpp"
#include "Structs.hpp"
#include "RaylibRenderer.hpp"


int main(void)
{
    RaylibRenderer renderer;
    Menu menu(renderer);
    
    renderer.initWindow(1920, 1080, "Gringo Engine");

    while (!WindowShouldClose() && menu.isRunning()) {
        renderer.startDrawing();
        renderer.clearScreen(ColorRGBA{0, 0, 0, 255});
        // menu.update();
        menu.update();
        renderer.endDrawing();
    }

    return 0;
}