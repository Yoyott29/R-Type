/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** TemplateMenu
*/

#include "TemplateMenu.hpp"

TemplateMenu::TemplateMenu(IRenderer& renderer) : AMenu(renderer), _template("default")
{
    initTexts();
    initButtons();
}

void TemplateMenu::initTexts()
{
    addText("Template", {960, 200}, 60, "Choose your Template");
}

void TemplateMenu::initButtons()
{
    addButton("2D", {450,  400}, 300, 100, 30, [this]() { _switchMenu = "EditorMenu"; setTemplate("2D"); });
    addButton("Multiplayer 2D", {790,  400}, 300, 100, 30, [this]() { _switchMenu = "EditorMenu"; setTemplate("Multiplayer 2D"); });
    addButton("Platformer 2D", {1130, 400}, 300, 100, 30, [this]() { _switchMenu = "EditorMenu"; setTemplate("Platformer 2D"); });
    addButton("Endless Runner 2D", {1470, 400}, 300, 100, 30, [this]() { _switchMenu = "EditorMenu"; setTemplate("Endless Runner 2D"); });
    addButton("Back", {1800, 50}, 200, 50, 20, [this]() { _switchMenu = "MainMenu"; });
}


