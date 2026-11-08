/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** MainMenu
*/

#include "MainMenu.hpp"

MainMenu::MainMenu(IEditorRenderer &renderer) : AMenu(renderer)
{
    _currentMenu = MenuType::MainMenu;

    MainMenu::initTexts();
    MainMenu::initButtons();
    // The pixel art logo (148 x 144), twice its size, above the title
    addImage("logo", {960, 180}, 296, 288, "gringo_logo.png");
}

void MainMenu::initTexts()
{
    addText("title", {960, 370}, 60, "GRINGO ENGINE", TEXT_COLOR);
    addText("subtitle", {960, 420}, 30, "Welcome to the Gringo Engine!", TEXT_COLOR);
}

void MainMenu::initButtons()
{
    addButton(
        "New Game", {660, 540}, 300, 100, 30, [this]() { _switchMenu = MenuType::TemplateMenu; },
        BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR);
    addButton(
        "Load Game", {1260, 540}, 300, 100, 30, [this]() { _switchMenu = MenuType::LoadMenu; },
        BUTTON_COLOR1, BUTTON_COLOR3, TEXT_COLOR);
    addButton(
        "Exit", {1800, 50}, 200, 50, 20, [this]() { _isRunning = false; }, BUTTON_COLOR1,
        BUTTON_COLOR3, TEXT_COLOR);
}

void MainMenu::initRectangles() {}
