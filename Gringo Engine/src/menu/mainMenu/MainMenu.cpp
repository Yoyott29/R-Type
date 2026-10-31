/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** MainMenu
*/

#include "MainMenu.hpp"

MainMenu::MainMenu(IRenderer &renderer) : AMenu(renderer)
{
    initTexts();
    initButtons();
}

void MainMenu::initTexts()
{
    addText("title", {960, 300}, 60, "GRINGO ENGINE", TEXT_COLOR);
    addText("subtitle", {960, 350}, 30, "Welcome to the Gringo Engine!", TEXT_COLOR);
}

void MainMenu::initButtons()
{
    addButton(
        "New Game", {660, 540}, 300, 100, 30, [this]() { _switchMenu = "TemplateMenu"; },
        BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR);
    addButton("Load Game", {1260, 540}, 300, 100, 30, nullptr, BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR);
    addButton(
        "Exit", {1800, 50}, 200, 50, 20, [this]() { _isRunning = false; }, BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR);
}

void MainMenu::initRectangles() {}
